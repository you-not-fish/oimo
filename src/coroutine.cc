#include <cassert>
#include "log.h"
#include "coroutine.h"

namespace Oimo {
    uint64_t Coroutine::t_coroutineID = 0;
    Coroutine::sPtr Coroutine::t_currentCoroutine = nullptr;
    Coroutine::sPtr Coroutine::t_mainCoroutine = nullptr;

    Coroutine::Coroutine() {
        assert(t_coroutineID == 0);
        m_id = t_coroutineID++;
    }

    Coroutine::~Coroutine() {
        if (m_state == CoroutineState::RUNNING) {
            
        }
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
        t_currentCoroutine->m_state = CoroutineState::SUSPENDED;
        setCurrentCoroutine(t_mainCoroutine);
        coctx_swap(&t_currentCoroutine->m_ctx, &t_mainCoroutine->m_ctx);
    }

    void Coroutine::yieldToStopped() {
        if (isMainCoroutine()) {
            LOG_FATAL << "Main coroutine should never yield";
        }
        if (t_currentCoroutine->m_state != CoroutineState::RUNNING) {
            LOG_FATAL << "Coroutine should never yield when not running";
        }
        t_currentCoroutine->m_state = CoroutineState::STOPPED;
        setCurrentCoroutine(t_mainCoroutine);
        coctx_swap(&t_currentCoroutine->m_ctx, &t_mainCoroutine->m_ctx);
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

    void Coroutine::run(Coroutine::sPtr self) {
        assert(self);
        assert(self->m_state == CoroutineState::RUNNING);
        assert(self == t_currentCoroutine);
        self->m_func();
        yieldToStopped();
        LOG_FATAL << "Coroutine should never reach here";

    }
}