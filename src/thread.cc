#include "thread.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <cassert>

namespace Oimo {
    thread_local pid_t t_currentThreadID = 0;
    thread_local std::string t_currentThreadName = "unknown";

    Thread::~Thread() {
        if (m_state == ThreadState::RUNNING) {
            stop();
        }
    }

    void Thread::start() {
        assert(m_state == ThreadState::INIT);
        m_thread = std::thread(run, this);
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock);
    }

    void Thread::stop() {
        assert(m_state != ThreadState::INIT);
        join();
        m_state = ThreadState::STOPPED;
    }

    void Thread::join() {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    void Thread::run(void* arg) {
        Thread* self = static_cast<Thread*>(arg);
        t_currentThreadID = syscall(SYS_gettid);
        if (!self->m_name.empty()) {
            t_currentThreadName = self->m_name;
        }
        self->m_state = ThreadState::RUNNING;
        self->m_cond.notify_one();
        self->m_func();
        self->m_state = ThreadState::STOPPED;
    }
} // namespace Oimo