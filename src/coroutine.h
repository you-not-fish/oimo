#pragma once

#include <atomic>
#include <string>
#include <memory>
#include <functional>
#include "coctx.h"
#include "thread.h"
#include "noncopyable.h"

namespace Oimo {
    class Coroutine : noncopyable,
        public std::enable_shared_from_this<Coroutine> {
        friend class Thread;
        friend class Application;
    public:
        using sPtr = std::shared_ptr<Coroutine>;
        using SessionID = uint16_t;
        using CoroutineFunc = std::function<void()>;
        Coroutine(CoroutineFunc func, uint32_t stackSize = 0);
        ~Coroutine();
        enum class CoroutineState {
            INIT,
            RUNNING,
            SUSPENDED,
            STOPPED
        };
        void setState(CoroutineState state) { m_state = state; }
        CoroutineState state() const { return m_state; }
        void resume();
        void reset(CoroutineFunc func, uint32_t stackSize = 0);
        SessionID sid() const { return m_sid; }
        void setSid(SessionID sid) { m_sid = sid; }
        void setCallback(CoroutineFunc func) { m_func = func; }
        static SessionID generateSid();
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
        void initContext();
        static void run(Coroutine *self);
        static thread_local uint64_t t_coroutineID;
        static thread_local Coroutine::sPtr t_currentCoroutine;
        static thread_local Coroutine::sPtr t_mainCoroutine;
        uint64_t m_id;
        SessionID m_sid {0};
        CoroutineFunc m_func;
        struct coctx m_ctx;
        uint32_t m_stackSize;
        char* m_stack {nullptr};
        char* m_stackTop {nullptr};
        CoroutineState m_state{CoroutineState::INIT};
    };
} // namespace Oimo