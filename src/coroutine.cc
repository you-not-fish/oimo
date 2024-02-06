#include <cassert>
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
        assert(coroutine);
        assert(coroutine->m_state == CoroutineState::SUSPENDED);
        coroutine->m_state = CoroutineState::RUNNING;
        coctx_swap(&t_currentCoroutine->m_ctx, &coroutine->m_ctx);
    }

    void Coroutine::yieldToSuspend() {
        t_currentCoroutine->m_state = CoroutineState::SUSPENDED;
        coctx_swap(&t_currentCoroutine->m_ctx, &t_mainCoroutine->m_ctx);
    }

    void Coroutine::yieldToStopped() {
        t_currentCoroutine->m_state = CoroutineState::STOPPED;
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
        assert(m_state == CoroutineState::INIT);
    }
}