#include <cassert>
#include <errno.h>
#include <sys/epoll.h>
#include "sysMsg.h"
#include "socketState.h"
#include "address.h"
#include"epoller.h"
#include "serviceContextMgr.h"
#include "protobuf/newConn.pb.h"
#include "log.h"
#include "sysMsg.h"
#include "socketContext.h"

namespace Oimo {
namespace Net {

    SocketContext::SocketContext(int fd, uint32_t serv)
        : FdContext(fd)
        , m_sock(fd)
        , m_serv(serv)
        , m_readSize(1024)
        , m_sockType(SocketType::NEW)
        , m_closing(false)
        , m_closeSession(0) {}

    SocketContext::~SocketContext() {}
    void SocketContext::handleEvent() {
        int err = 0;
        do {
            if (m_revents & EPOLLHUP) {
                LOG_TRACE << "epoll hup event on fd " << m_fd
                        << ", socktype: " << SocketType2String(m_sockType)
                        << ", revents: " << eventsToStr(m_revents)
                        << ", events: " << eventsToStr(m_events);
                disableAll();
                m_sockType = SocketType::CLOSE;
                err = -1;
                break;
            }
            switch (m_sockType) {
                case SocketType::LISTEN:
                    assert(m_revents & EPOLLIN);
                    err = newConnection();
                    break;
                default:
                    if (m_revents & EPOLLIN) {
                        err = handleRead();
                    }
                    if (m_revents & EPOLLOUT) {
                        err += handleWrite();
                    }
                    break;
            }
        } while (0);
        if (err < 0) {
            Oimo::Packle::sPtr Packle = std::make_shared<Oimo::Packle>(
                (Oimo::Packle::MsgID)SystemMsgID::ERROR);
            Packle->setFd(m_fd);
            sendProto(Packle, m_serv);
        }

    }

    void SocketContext::reset(int fd, uint32_t serv) {
        assert(fd == -1 || !isValid());
        m_sock.close();
        FdContext::reset();
        m_fd = fd;
        m_sock.setFd(fd);
        m_serv = serv;
        m_sockType = SocketType::NEW;
        m_wbList.clear();
        m_readSize = 1024;
        m_closing = false;
        m_closeSession = 0;
    }

    int SocketContext::newConnection() {
        Address addr;
        int connFd = m_sock.accept(&addr);
        if (connFd < 0) {
            LOG_ERROR << "accept error: " << ::strerror(errno);
            return 0;
        }
        
        LOG_TRACE << "new connection from " << addr.ipAsString()
            << ":" << addr.portAsString();
        NetProto::NewConn newConn;
        newConn.set_fd(connFd);
        newConn.set_ip(addr.ipForNet());
        newConn.set_port(addr.portForNet());
        Packle::sPtr packle = std::make_shared<Packle>(
            (Packle::MsgID)SystemMsgID::NEWCONN);
        packle->serialize(newConn);
        sendProto(packle, m_serv);
        return 0;
    }

    int SocketContext::handleRead() {
        char *buf = new char[m_readSize];
        ssize_t n;
        for (;;) {
            n = ::read(m_fd, buf, m_readSize);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                std::string err = ::strerror(errno);
                LOG_ERROR << "read error: " << err; //::strerror(errno);
                return -1;
            }
            break;
        }
        
        if (n == 0) {
            if (m_sockType == SocketType::HALFCLOSE_READ ||
                m_sockType == SocketType::CLOSE) {
                LOG_TRACE << "read endpoint has been closed";
                return 0;
            }
            LOG_TRACE << "peer has closed the connection";
            shutRead();
            disableRead();
            if (!m_closing) {
                Oimo::Packle::sPtr packle = std::make_shared<Oimo::Packle>(
                    (Oimo::Packle::MsgID)SystemMsgID::CLOSEREAD);
                packle->setFd(m_fd);
                sendProto(packle, m_serv);
            }
            return 0;
        }
        if (m_sockType == SocketType::HALFCLOSE_READ ||
            m_sockType == SocketType::CLOSE) {
            delete[] buf;
            return 0;
        }
        LOG_TRACE << "read " << n << " bytes";
        if (n == m_readSize) {
            m_readSize *= 2;
        } else if (n < m_readSize / 2) {
            m_readSize /= 2;
        }
        Packle::sPtr packle = std::make_shared<Packle>(
            (Packle::MsgID)SystemMsgID::DATA);
        packle->setFd(m_fd);
        packle->setBuf(buf);
        packle->setSize(n);
        sendProto(packle, m_serv);
        return 0;
    }

    int SocketContext::writeWB() {
        while (!m_wbList.empty()) {
            auto &wb = m_wbList.front();
            ssize_t n = ::write(m_fd, wb.buf + wb.pos, wb.size - wb.pos);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    return 1;
                }
                LOG_ERROR << "write error: " << ::strerror(errno);
                return -1;
            }
            wb.pos += n;
            if (wb.pos == wb.size) {
                delete[] wb.buf;
                m_wbList.pop_front();
            }
        }
        return 0;
    }

    int SocketContext::handleWrite() {
        int ret = writeWB();
        if (m_closeSession) {
            LOG_TRACE << "all data sent, closing...";
            assert(m_closing);
            disableAll();
            m_sockType = SocketType::CLOSE;
            freeWB();
            sendClosedPackle(m_closeSession);
            return 0;
        }
        if (ret == 0) {
            if (m_wbList.empty()) {
                disableWrite();
            }
        } else if (ret == -1) {
            LOG_TRACE << "write error, closing...";
            shutWrite();
            disableWrite();
            return -1;
        }
        return 0;
    }

    size_t SocketContext::appendWB(char *buf, size_t len) {
        m_wbList.push_back({buf, len, 0});
        if (!isWriting()) {
            enableWrite();
        }
        return len;
    }

    bool SocketContext::shutRead() {
        if (m_sockType == SocketType::HALFCLOSE_READ ||
            m_sockType == SocketType::CLOSE) {
            return true;
        }
        m_sockType = m_sockType == SocketType::HALFCLOSE_WRITE ?
            SocketType::CLOSE : SocketType::HALFCLOSE_READ;
        return m_sock.shutdownRead();
    }

    bool SocketContext::shutWrite() {
        if (m_sockType == SocketType::HALFCLOSE_WRITE ||
            m_sockType == SocketType::CLOSE) {
            return true;
        }
        m_sockType = m_sockType == SocketType::HALFCLOSE_READ ?
            SocketType::CLOSE : SocketType::HALFCLOSE_WRITE;
        return m_sock.shutdownWrite();
    }

    void SocketContext::freeWB() {
        for (auto &wb : m_wbList) {
            delete[] wb.buf;
        }
        m_wbList.clear();
    }

    bool SocketContext::close(uint16_t session) {
        if (m_sockType == SocketType::CLOSE) {
            freeWB();
            sendClosedPackle(session);
            return false;
        }
        shutRead();
        disableRead();
        if (m_wbList.empty()) {
            LOG_TRACE << "no data to send, closing...";
            disableAll();
            m_sockType = SocketType::CLOSE;
            freeWB();
            sendClosedPackle(session);
        } else {
            LOG_TRACE << "data to send, closing after all data sent...";
            m_closeSession = session;
            if (!isWriting()) {
                enableWrite();
            }
        }
        return true;
    }

    void SocketContext::sendClosedPackle(uint16_t session) {
        int fd = m_fd;
        uint32_t serv = m_serv;
        m_fd = -1;
        m_serv = 0;
        m_sock.close();
        assert(!isValid());
        Oimo::Packle::sPtr packle = std::make_shared<Oimo::Packle>(
            (Oimo::Packle::MsgID)SystemMsgID::CLOSED);
        packle->setSessionID(session);
        packle->setFd(fd);
        packle->setIsRet(true);
        sendProto(packle, serv);
    }

}  // Net
} // Oimo