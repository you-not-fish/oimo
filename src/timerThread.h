#pragma once

#include "thread.h"

namespace Oimo {
    class TimeThread {
    public:
        TimeThread();
        ~TimeThread();
        void start();
        void stop();
        void join();
    private:
        void run();
        Thread::sPtr m_thread;
    };
}