#pragma once

#include "thread.h"

namespace Oimo {
namespace Net {
    class SocketThread {
    public:
        SocketThread();
        ~SocketThread();
        void start();
        void join();
        void stop();
    private:
        void run();
        Thread::sPtr m_thread;
    };
} // Net
} // Oimo