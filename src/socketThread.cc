#include "socketThread.h"
#include "socketServer.h"
#include "singleton.h"

namespace Oimo {
namespace Net {
    SocketThread::SocketThread()
        : m_thread(
            std::make_shared<Thread>(
                std::bind(&SocketThread::run, this),
                "SocketThread")
            ) {
    }

    SocketThread::~SocketThread() {
        stop();
        join();
    }

    void SocketThread::start() {
        auto& server = Singleton<SocketServer>::instance();
        server.eventLoop()->loop();
    }

    void SocketThread::join() {
        m_thread->join();
    }

    void SocketThread::stop() {
        auto& server = Singleton<SocketServer>::instance();
        server.eventLoop()->stop();
    }
} // Net
} // Oimo