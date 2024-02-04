#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "log.h"

namespace Oimo {
    class LogThread {
    public:
        LogThread();
        ~LogThread();
        void start();
        void stop();
    private:
        void threadFunc();
        std::vector<LogBuffer::sPtr> m_buffers;
        std::thread m_thread;
        bool m_running;
        std::mutex m_mutex;
        std::condition_variable m_cond;
    };
}