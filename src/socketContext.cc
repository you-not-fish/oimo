#include <cassert>
#include <errno.h>
#include <sys/epoll.h>
#include "sysMsg.h"
#include "socketState.h"
#include "address.h"
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
        , m_sockType(SocketType::NEW) {}

    SocketContext::~SocketContext() {}
    void SocketContext::handleEvent() {
        if (m_revents & EPOLLHUP) {
            LOG_ERROR << "socket error";
            return;
        }
        switch (m_sockType) {
            case SocketType::LISTEN:
                assert(m_revents & EPOLLIN);
                newConnection();
                break;
            case SocketType::ACCEPT:
                if (m_revents & EPOLLIN) {
                    handleRead();
                }
                if (m_revents & EPOLLOUT) {
                    handleWrite();
                }
                break;
            default:
                break;
        }
    }

    void SocketContext::reset(int fd, uint32_t serv) {
        if (isValid()) {
            m_sock.close();
            m_fd = -1;
            assert(!isValid());
        }
        m_fd = fd;
        m_sock.setFd(fd);
        m_serv = serv;
        m_sockType = SocketType::NEW;
        m_wbList.clear();
    }

    void SocketContext::newConnection() {
        Address addr;
        int connFd = m_sock.accept(&addr);
        if (connFd < 0) {
            LOG_ERROR << "accept error: " << ::strerror(errno);
            return;
        }
        LOG_DEBUG << "new connection from " << addr.ipAsString()
            << ":" << addr.portAsString();
        NetProto::NewConn newConn;
        newConn.set_fd(connFd);
        newConn.set_ip(addr.ipForNet());
        newConn.set_port(addr.portForNet());
        Packle::sPtr packle = std::make_shared<Packle>(
            (Packle::MsgID)SystemMsgID::NEWCONN);
        packle->serialize(newConn);
        sendProto(packle, m_serv);
    }

    void SocketContext::handleRead() {
        char *buf = new char[m_readSize];
        ssize_t n;
        for (;;) {
            n = ::read(m_fd, buf, m_readSize);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                LOG_ERROR << "read error: " << ::strerror(errno);
                return;
            }
            break;
        }
        
        if (n == 0) {
            LOG_DEBUG << "peer closed";
            m_sock.shutdownRead();
            disableRead();
            return;
        }
        LOG_DEBUG << "read " << n << " bytes";
        if (n == m_readSize) {
            m_readSize *= 2;
        } else if (n < m_readSize / 2) {
            m_readSize /= 2;
        }
        Packle::sPtr packle = std::make_shared<Packle>(
            (Packle::MsgID)SystemMsgID::DATA);
        packle->setSessionID(m_fd);
        packle->setBuf(buf);
        packle->setSize(n);
        sendProto(packle, m_serv);
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

    void SocketContext::handleWrite() {
        int ret = writeWB();
        if (ret == 0) {
            disableWrite();
        } else if (ret < 0) {
            // TODO
        }
    }

    size_t SocketContext::appendWB(char *buf, size_t len) {
        m_wbList.push_back({buf, len, 0});
        if (!isWriting()) {
            enableWrite();
        }
        return len;
    }

}  // Net
} // Oimo