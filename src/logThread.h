#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "log.h"
#include "thread.h"

namespace Oimo {
    class LogThread : noncopyable {
    public:
        LogThread();
        ~LogThread();
        void start();
        void join();
        void stop();
    private:
        void threadFunc();
        std::vector<LogBuffer::sPtr> m_buffers;
        bool m_running;
        Thread::sPtr m_thread;
        LogFile::uPtr m_file;\
        bool m_appendToFile;
        bool m_appendToStdout;
    };
}