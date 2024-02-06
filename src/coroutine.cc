#include <cassert>
#include "log.h"
#include "singleton.h"
#include "config.h"
#include "coroutine.h"

namespace Oimo {
    thread_local uint64_t Coroutine::t_coroutineID = 0;
    thread_local Coroutine::sPtr Coroutine::t_currentCoroutine = nullptr;
    thread_local Coroutine::sPtr Coroutine::t_mainCoroutine = nullptr;

    Coroutine::Coroutine(CoroutineFunc func, uint32_t stackSize)
        : m_func(func)
        , m_state(CoroutineState::INIT) {
        assert(t_mainCoroutine);
        m_id = t_coroutineID++;
        m_stackSize = stackSize ? stackSize :
            Singleton<Config>::instance().get<uint32_t>(
                "coroutine.stack_size", 1024 * 1024);
        m_stack = new char[m_stackSize];
        m_stackTop = m_stack + m_stackSize;

        memset(&m_ctx, 0, sizeof(m_ctx));

        m_ctx.regs[static_cast<int>(Register::kRSP)]
            = m_stackTop;
        m_ctx.regs[static_cast<int>(Register::kRBP)]
            = m_stackTop;
        m_ctx.regs[static_cast<int>(Register::kRETAddr)]
            = reinterpret_cast<char*>(run);
        m_ctx.regs[static_cast<int>(Register::kRDI)]
            = reinterpret_cast<char*>(this);
    }

    Coroutine::Coroutine() {
        assert(t_coroutineID == 0);
        m_id = t_coroutineID++;
        m_stackSize = 0;
        m_state = CoroutineState::RUNNING;
    }

    Coroutine::~Coroutine() {
        if (!isMainCoroutine() &&
            m_state == CoroutineState::RUNNING) {
            LOG_ERROR << "Coroutine is still running when destroyed";
        }
        if (m_stack) {
            delete[] m_stack;
        }
    }

    void Coroutine::resume() {
        resume(shared_from_this());
    }

    void Coroutine::reset(CoroutineFunc func, uint32_t stackSize) {
        m_func = func;
        if (stackSize) {
            m_stackSize = stackSize;
        }
        m_state = CoroutineState::INIT;
    }

    void Coroutine::resume(Coroutine::sPtr coroutine) {
        if (coroutine == t_mainCoroutine) {
            LOG_FATAL << "Main coroutine should never be resumed";
        }
        if (t_currentCoroutine != t_mainCoroutine) {
            LOG_FATAL << "Coroutine should never be resumed in another coroutine";
        }
        if (coroutine->m_state == CoroutineState::RUNNING) {
            LOG_FATAL << "Coroutine should never be resumed when running";
        }
        if (coroutine->m_state == CoroutineState::STOPPED) {
            LOG_FATAL << "Coroutine should never be resumed when stopped";
        }
        coroutine->m_state = CoroutineState::RUNNING;
        setCurrentCoroutine(coroutine);
        coctx_swap(&t_mainCoroutine->m_ctx, &coroutine->m_ctx);
    }

    bool Coroutine::isMainCoroutine() {
        return t_currentCoroutine == t_mainCoroutine;
    }

    void Coroutine::yieldToSuspend() {
        if (isMainCoroutine()) {
            LOG_FATAL << "Main coroutine should never yield";
        }
        if (t_currentCoroutine->m_state != CoroutineState::RUNNING) {
            LOG_FATAL << "Coroutine should never yield when not running";
        }
        auto cor = t_currentCoroutine;
        cor->m_state = CoroutineState::SUSPENDED;
        setCurrentCoroutine(t_mainCoroutine);
        coctx_swap(&cor->m_ctx, &t_mainCoroutine->m_ctx);
    }

    void Coroutine::yieldToStopped() {
        if (isMainCoroutine()) {
            LOG_FATAL << "Main coroutine should never yield";
        }
        if (t_currentCoroutine->m_state != CoroutineState::RUNNING) {
            LOG_FATAL << "Coroutine should never yield when not running";
        }
        auto cor = t_currentCoroutine;
        cor->m_state = CoroutineState::STOPPED;
        setCurrentCoroutine(t_mainCoroutine);
        coctx_swap(&cor->m_ctx, &t_mainCoroutine->m_ctx);
    }

    Coroutine::sPtr Coroutine::currentCoroutine() {
        return t_currentCoroutine;
    }

    Coroutine::sPtr Coroutine::mainCoroutine() {
        return t_mainCoroutine;
    }

    void Coroutine::setCurrentCoroutine(Coroutine::sPtr coroutine) {
        t_currentCoroutine = coroutine;
    }

    uint64_t Coroutine::currentCoroutineID() {
        return t_currentCoroutine->m_id;
    }

    void Coroutine::run(Coroutine* self) {
        assert(self);
        assert(self->m_state == CoroutineState::RUNNING);
        assert(self == t_currentCoroutine.get());
        self->m_func();
        yieldToStopped();
        LOG_FATAL << "Coroutine should never reach here";

    }
}