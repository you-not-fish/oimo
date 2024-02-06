#pragma once

#include <atomic>
#include <string>
#include <memory>
#include <functional>
#include "coctx.h"
#include "thread.h"
#include "init.h"
#include "noncopyable.h"

namespace Oimo {
    class Coroutine : noncopyable,
    std::enable_shared_from_this<Coroutine> {
        friend class Thread;
        friend void initOimo();
    public:
        using sPtr = std::shared_ptr<Coroutine>;
        using CoroutineFunc = std::function<void()>;
        ~Coroutine();
        enum class CoroutineState {
            INIT,
            RUNNING,
            SUSPENDED,
            STOPPED
        };
        void reset(CoroutineFunc func, uint32_t stackSize = 0);
        static bool isMainCoroutine();
        static void resume(Coroutine::sPtr coroutine);
        static void yieldToSuspend();
        static void yieldToStopped();
        static Coroutine::sPtr currentCoroutine();
        static Coroutine::sPtr mainCoroutine();
        static void setCurrentCoroutine(Coroutine::sPtr coroutine);
        static uint64_t currentCoroutineID();
    private:
        Coroutine();
        static void run(Coroutine::sPtr self);
        static thread_local uint64_t t_coroutineID;
        static thread_local Coroutine::sPtr t_currentCoroutine;
        static thread_local Coroutine::sPtr t_mainCoroutine;
        uint64_t m_id;
        CoroutineFunc m_func;
        struct coctx m_ctx;
        uint32_t m_stackSize;
        CoroutineState m_state{CoroutineState::INIT};
    };
} // namespace Oimo