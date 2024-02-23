#pragma once

#include <vector>
#include <list>
#include <memory>
#include <chrono>
#include "spinLock.h"
#include "singleton.h"

namespace Oimo {
    struct Timer {
        using sPtr = std::shared_ptr<Timer>;
        using wPtr = std::weak_ptr<Timer>;
        uint32_t delay;
        uint32_t interval;
        uint32_t serv;
        uint16_t session;
        int level;
        bool isRepeat() const { return interval > 0; }
    };

    class TimeWheel {
    public:
        using TimerList = std::list<Timer::wPtr>;
        TimeWheel();
        ~TimeWheel();

        void run();
        void stop();

        void addTimer(Timer::wPtr timer);

    private:
        void add(Timer::wPtr timer);
        void tick();
        void hold();
        void execute(uint32_t serv, uint16_t session);
        uint64_t m_cur;
        std::chrono::steady_clock::time_point m_last;
        bool m_running;
        int m_wheelSize;
        int m_interval;
        std::vector<TimerList> m_wheel;
        std::vector<Timer::wPtr> m_timers;
        SpinLock m_lock;
    };

    using GTimerWheel = Singleton<TimeWheel>;
}
