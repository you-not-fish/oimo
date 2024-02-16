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
        Singleton<SocketServer>::instance().sendCtrl(
            reinterpret_cast<char*>(ctrl.head+6), len+2
        );
        Oimo::Coroutine::yieldToSuspend();
    }

    void Connection::close() {
        // TODO
    }

    size_t Connection::send(const char* data, size_t len) {
        // TODO
        return 0;
    }

    size_t Connection::recv(char* data, size_t len) {
        // TODO
        return 0;
    }
} // Net
} // Oimo