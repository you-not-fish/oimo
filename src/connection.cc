#include <cassert>
#include <cstring>
#include "connection.h"
#include "serviceContext.h"
#include "ctrlMsg.h"
#include "socketServer.h"
#include "tcpServer.h"
#include "singleton.h"

namespace Oimo {
namespace Net {
    void Connection::start() {
        struct CtrlPacket ctrl;
        uint8_t len = sizeof(struct StartCtrl);
        ctrl.head[6] = (uint8_t)'S';
        ctrl.head[7] = len;
        ctrl.msg.start.fd = m_fd;
        auto self = Oimo::ServiceContext::currentContext();
        Oimo::Coroutine::SessionID sid = self->getSession();
        ctrl.msg.start.session = sid;
        auto cor = Oimo::Coroutine::currentCoroutine();
        cor->setSid(sid);
        self->suspend(cor);
        GSocketServer::instance().sendCtrl(
            reinterpret_cast<char*>(ctrl.head+6), len+2
        );
        Oimo::Coroutine::yieldToSuspend();
    }

    void Connection::close() {
        auto ctx = GSocketServer::instance().getSocketContext(m_fd);
        if (ctx->isClosing()) {
            return;
        }
        setCloseFlag();
        ctx->setCloseFlag();
        struct CtrlPacket ctrl;
        uint8_t len = sizeof(struct CloseCtrl);
        ctrl.head[6] = (uint8_t)'C';
        ctrl.head[7] = len;
        auto self = Oimo::ServiceContext::currentContext();
        auto sid = self->getSession();
        auto cor = Oimo::Coroutine::currentCoroutine();
        cor->setSid(sid);
        ctrl.msg.close.fd = m_fd;
        ctrl.msg.close.session = sid;
        m_serv->removeConn(m_fd);
        GSocketServer::instance().sendCtrl(
            reinterpret_cast<char*>(ctrl.head+6), len+2
        );
        self->suspend(cor);
        Oimo::Coroutine::yieldToSuspend();
        m_fd = -1;
    }

    int Connection::sendToSocket(const char *buf, size_t len, bool needCopy) {
        if (m_fd == -1) {
            return -1;
        }
        auto ctx = GSocketServer::instance().getSocketContext(m_fd);
        if (ctx->isClosing()) {
            return -1;
        }
        char *data = nullptr;
        if (needCopy) {
            data = new char[len];
            ::memcpy(data, buf, len);
        } else {
            data = const_cast<char*>(buf);
        }
        struct CtrlPacket ctrl;
        uint8_t size = sizeof(struct DataCtrl);
        ctrl.head[6] = (uint8_t)'D';
        ctrl.head[7] = size;
        ctrl.msg.data.fd = m_fd;
        ctrl.msg.data.buf = data;
        ctrl.msg.data.len = len;
        auto self = Oimo::ServiceContext::currentContext();
        GSocketServer::instance().sendCtrl(
            reinterpret_cast<char*>(ctrl.head+6), size+2
        );
        return len;
    }

    size_t Connection::send(const char* data, size_t len) {
        return sendToSocket(data, len, true);
    }

    size_t Connection::send(Oimo::Packle::sPtr packle) {
        auto size = packle->size();
        auto buf = packle->getAndResetBuf();
        return sendToSocket(buf, size, false);
    }

    size_t Connection::recv(char* data, size_t len) {
        while (m_buffer.used() == 0) {
            if (m_closing) {
                return 0;
            }
            auto cor = Oimo::Coroutine::currentCoroutine();
            m_cors.push_back(cor);
            Oimo::Coroutine::yieldToSuspend();
        }
        return m_buffer.read(data, len);
    }

    size_t Connection::recvN(char* data, size_t len) {
        size_t n = 0;
        while (n < len) {
            size_t t = recv(data+n, len-n);
            n += t;
            if (t == 0) {
                break;
            }
        }
        return n;
    }

    size_t Connection::append(const char* data, size_t len) {
        size_t n = m_buffer.write(data, len);
        assert(n == len);
        for (auto& cor : m_cors) {
            Oimo::ServiceContext::currentContext()->addFork(cor);
        }
        m_cors.clear();
        return n;
    }
    
    bool Connection::isClosing() const {
        auto ctx = GSocketServer::instance().getSocketContext(m_fd);
        if (!ctx->isValid()) {
            return true;
        }
        return ctx->isClosing();
    }
} // Net
} // Oimo