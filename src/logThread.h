#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "log.h"
#include "thread.h"

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
        Thread m_thread;
        bool m_running;
        LogFile::uPtr m_file;\
        bool m_appendToFile;
        bool m_appendToStdout;
    };
}