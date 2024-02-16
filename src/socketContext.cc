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
        , m_sockType(SocketType::NEW)
        , m_sending(0) {}

    SocketContext::~SocketContext() {}
    void SocketContext::handleEvent() {
        switch (m_sockType) {
            case SocketType::LISTEN:
                assert(m_revents & EPOLLIN);
                newConnection();
                break;
            case SocketType::ACCEPT:
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
        m_sending = 0;
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

}  // Net
} // Oimo