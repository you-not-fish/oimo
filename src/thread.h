#pragma once
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <unistd.h>
#include "log.h"

namespace Oimo {
    extern thread_local pid_t t_currentThreadID;
    extern thread_local std::string t_currentThreadName;

    class Thread {
    public:
        using sPtr = std::shared_ptr<Thread>;
        using ThreadFunc = std::function<void()>;

        Thread(ThreadFunc func, const std::string& name = std::string())
            : m_name(name)
            , m_func(func)
        {
        }

        ~Thread();
        void start();
        void stop();
        void join();
        const std::string& name() const { return m_name; }
        void setName(const std::string& name) { m_name = name; }
        static pid_t currentThreadID() { return t_currentThreadID; }
        static const std::string& currentThreadName() { return t_currentThreadName; }
        static bool isMainThread() { return currentThreadID() == ::getpid(); }
        enum class ThreadState {
            INIT,
            RUNNING,
            STOPPED
        };
        ThreadState state() const { return m_state; }
    private:
        static void run(void* arg);
        std::string m_name;
        ThreadFunc m_func;
        std::thread m_thread;
        std::mutex m_mutex;
        std::condition_variable m_cond;
        ThreadState m_state{ThreadState::INIT};
    };
} // namespace Oimo