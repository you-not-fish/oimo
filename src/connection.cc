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
        Oimo::Coroutine::SessionID sid = Oimo::Coroutine::generateSid();
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
        auto sid = Oimo::Coroutine::generateSid();
        auto cor = Oimo::Coroutine::currentCoroutine();
        cor->setSid(sid);
        ctrl.msg.close.fd = m_fd;
        ctrl.msg.close.session = Oimo::Coroutine::generateSid();
        auto self = Oimo::ServiceContext::currentContext();
        m_serv->removeConn(m_fd);
        GSocketServer::instance().sendCtrl(
            reinterpret_cast<char*>(ctrl.head+6), len+2
        );
        self->suspend(cor);
        Oimo::Coroutine::yieldToSuspend();
        m_fd = -1;
    }

    void Connection::sendToSocket(const char *buf, size_t len, bool needCopy) {
        auto ctx = GSocketServer::instance().getSocketContext(m_fd);
        if (ctx->isClosing()) {
            return;
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
    }

    size_t Connection::send(const char* data, size_t len) {
        sendToSocket(data, len, true);
        return len;
    }

    size_t Connection::send(Oimo::Packle::sPtr packle) {
        auto size = packle->size();
        auto buf = packle->getAndResetBuf();
        sendToSocket(buf, size, false);
        return size;
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

    size_t Connection::append(const char* data, size_t len) {
        size_t n = m_buffer.write(data, len);
        assert(n == len);
        for (auto& cor : m_cors) {
            Oimo::ServiceContext::currentContext()->addFork(cor);
        }
        m_cors.clear();
        return n;
    }
} // Net
} // Oimo