#include "socketThread.h"
#include "socketServer.h"
#include "singleton.h"

namespace Oimo {
namespace Net {
    SocketThread::SocketThread() {}

    SocketThread::~SocketThread() {
        stop();
        join();
    }

    void SocketThread::start() {
        m_thread.reset(new Thread(std::bind(&SocketThread::run, this), "socketthread"));
        m_thread->start();
    }

    void SocketThread::join() {
        m_thread->join();
    }

    void SocketThread::stop() {
        auto& server = GSocketServer::instance();
        server.eventLoop()->stop();
    }

    void SocketThread::run() {
        auto& server = GSocketServer::instance();
        server.eventLoop()->loop();
    }
} // Net
} // Oimo