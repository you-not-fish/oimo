#include <cassert>
#include "connection.h"
#include "serviceContext.h"
#include "ctrlMsg.h"
#include "socketServer.h"
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
        // TODO
    }

    size_t Connection::send(const char* data, size_t len) {
        auto ctx = GSocketServer::instance().getSocketContext(m_fd);
        assert(ctx->isValid());
        ctx->write(data, len, true);
        return 0;
    }

    size_t Connection::send(Oimo::Packle::sPtr packle) {
        auto ctx = GSocketServer::instance().getSocketContext(m_fd);
        assert(ctx->isValid());
        auto size = packle->size();
        auto buf = packle->getAndResetBuf();
        ctx->write(buf, size, false);
        return size;
    }

    size_t Connection::recv(char* data, size_t len) {
        while (m_buffer.used() == 0) {
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