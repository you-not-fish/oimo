#include "thread.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <cassert>
#include "coroutine.h"

namespace Oimo {
    thread_local int Thread::t_currentThreadID = 0;
    thread_local std::string Thread::t_currentThreadName = "unknown";

    Thread::~Thread() {
        if (m_state == ThreadState::RUNNING) {
            join();
        }
    }

    void Thread::start() {
        assert(m_state == ThreadState::INIT);
        assert(m_func);
        m_thread = std::thread(run, shared_from_this());
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock);
    }

    void Thread::join() {
        assert(m_state != ThreadState::INIT);
        if (m_thread.joinable()) {
            m_thread.join();
        }
        m_state = ThreadState::STOPPED;
    }

    void Thread::run(Thread::sPtr self) {
        assert(self);
        assert(self->m_state == ThreadState::INIT);
        t_currentThreadID = syscall(SYS_gettid);
        if (!self->m_name.empty()) {
            t_currentThreadName = self->m_name;
        }
        Coroutine::t_mainCoroutine.reset(new Coroutine());
        Coroutine::setCurrentCoroutine(Coroutine::t_mainCoroutine);
        self->m_state = ThreadState::RUNNING;
        self->m_cond.notify_one();
        self->m_func();
    }
} // namespace Oimo