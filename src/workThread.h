#pragma once

#include "thread.h"
#include <vector>

namespace Oimo {
    class WorkThread {
    public:
        WorkThread();
        ~WorkThread();

        void start();
        void join();
        void stop();

    private:
        static void work();
        static bool running;
        std::vector<Thread::sPtr> m_threads;
    };
}