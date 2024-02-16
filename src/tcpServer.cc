#include <cassert>
#include <sys/socket.h>
#include "address.h"
#include "service.h"
#include "log.h"
#include "singleton.h"
#include "sysMsg.h"
#include "coroutine.h"
#include "ctrlMsg.h"
#include "socketContext.h"
#include "socketServer.h"
#include "tcpServer.h"
#include "protobuf/newConn.pb.h"

namespace Oimo {
namespace Net {
    TcpServer::TcpServer(Oimo::Service* serv)
        : m_listenFd(-1)
        , m_serv(serv) {
        assert(m_serv != nullptr);
        
    }
    TcpServer::~TcpServer() {}

    void TcpServer::init() {
        m_serv->registerFunc((Packle::MsgID)SystemMsgID::NEWCONN,
            std::bind(&TcpServer::handleNewConn, this, std::placeholders::_1));
    }

    int TcpServer::initFd(const std::string& ip, uint16_t port) {
        if (m_listenFd != -1) {
            LOG_WARN << "TcpServer already initialized";
            return m_listenFd;
        }
        Address addr(ip, port);
        m_listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (m_listenFd < 0) {
            LOG_FATAL << "socket failed : " << strerror(errno);
            return -1;
        }
        
        auto ctx = Singleton<SocketServer>::instance()
            .getSocketContext(m_listenFd);
        assert(!ctx->isValid());
        ctx->reset(m_listenFd, m_serv->id());
        auto& sock = ctx->sock();
        if (!sock.bind(addr)) {
            ctx->close();
            return -1;
        }
        if (!sock.listen()) {
            ctx->close();
            return -1;
        }
        ctx->setSockType(SocketType::PLISTEN);
        return m_listenFd;
    }

    void TcpServer::start(ConnCb cb) {
        if (m_listenFd == -1) {
            LOG_FATAL << "TcpServer not initialized";
            return;
        }
        m_cb = cb;
        struct CtrlPacket ctrl;
        uint8_t len = sizeof(struct StartCtrl);
        ctrl.head[6] = (uint8_t)'S';
        ctrl.head[7] = len;
        ctrl.msg.start.fd = m_listenFd;
        Oimo::Coroutine::SessionID sid = Oimo::Coroutine::generateSid(m_serv->id());
        ctrl.msg.start.session = sid;
        auto self = Oimo::ServiceContext::currentContext();
        auto cor = Oimo::Coroutine::currentCoroutine();
        cor->setSid(sid);
        self->suspend(cor);
        Singleton<SocketServer>::instance().sendCtrl(
            reinterpret_cast<char*>(ctrl.head+6), len+2
        );
        Oimo::Coroutine::yieldToSuspend();
    }

    void TcpServer::handleNewConn(Packle::sPtr packle) {
        NetProto::NewConn newConn = packle->deserialize<NetProto::NewConn>();
        int fd = newConn.fd();
        uint32_t ip = newConn.ip();
        uint16_t port = newConn.port();
        Connection::sPtr conn = std::make_shared<Connection>(fd, ip, port);
        assert(m_conns.find(fd) == m_conns.end());
        m_conns[fd] = conn;
        auto ctx = Singleton<SocketServer>::instance().getSocketContext(fd);
        assert(!ctx->isValid());
        ctx->reset(fd, m_serv->id());
        ctx->setSockType(SocketType::PACCEPT);
        assert(m_cb);
        m_cb(conn);
    }
} // Net
}  // Oimo