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
            // m_sock.close();
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

    bool SocketContext::writeWB() {
        while (!m_wbList0.empty()) {
            auto &wb = m_wbList0.front();
            ssize_t n = ::write(m_fd, wb.buf + wb.pos, wb.size - wb.pos);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    return false;
                }
                LOG_ERROR << "write error: " << ::strerror(errno);
                return false;
            }
            wb.pos += n;
            if (wb.pos == wb.size) {
                delete[] wb.buf;
                m_wbList0.pop_front();
            }
        }
        return true;
    }

    void SocketContext::handleWrite() {
        if (writeWB()) {
            {
                Oimo::SpinLockGuard guard(m_lock);
                m_wbList0.swap(m_wbList);
            }
            writeWB();
            Oimo::SpinLockGuard guard(m_lock);
            if (m_wbList0.empty() && m_wbList.empty()) {
                disableWrite();
            }
        }
    }

    size_t SocketContext::write(const char *buffer, size_t len, bool needCopy) {
        char *buf = nullptr;
        if (needCopy) {
            buf = new char[len];
            ::memcpy(buf, buffer, len);
        } else {
            buf = const_cast<char*>(buffer);
        }
        Oimo::SpinLockGuard guard(m_lock);
        m_wbList.push_back({buf, len, 0});
        if (!isWriting()) {
            enableWrite();
        }
        return len;
    }

}  // Net
} // Oimo