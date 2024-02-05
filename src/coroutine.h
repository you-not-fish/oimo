#pragma once

#include <atomic>
#include <string>
#include <memory>
#include <functional>
#include "thread.h"

namespace Oimo {
    extern thread_local Coroutine::sPtr t_currentCoroutine;
    extern thread_local Coroutine::sPtr t_mainCoroutine;
    class Coroutine : noncopyable {
    public:
        using sPtr = std::shared_ptr<Coroutine>;
        using uPtr = std::unique_ptr<Coroutine>;
        using CoroutineFunc = std::function<void()>;
        Coroutine();
        ~Coroutine();
        void start();
        void stop();
        enum class CoroutineState {
            INIT,
            RUNNING,
            SUSPENDED,
            STOPPED
        };
    private:
        static void run(void* arg);
        uint64_t m_id;
        CoroutineFunc m_func;
        uint32_t m_stackSize;
        
        std::atomic<CoroutineState> m_state{CoroutineState::INIT};
    };
} // namespace Oimo