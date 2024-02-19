#include <thread>
#include "timeWheel.h"
#include "config.h"

namespace Oimo {
    TimeWheel::TimeWheel() : m_cur(0), m_running(false) {
        m_wheelSize = GConfig::instance().getInt("timeWheel.size", 60);
        m_interval = GConfig::instance().getInt("timeWheel.interval", 10);
        m_wheel.resize(m_wheelSize);
    }

    TimeWheel::~TimeWheel() {
        stop();
    }

    void TimeWheel::run() {
        m_running = true;
        m_last = std::chrono::steady_clock::now();
        while (m_running) {
            tick();
            hold();
        }
    }

    void TimeWheel::stop() {
        m_running = false;
    }

    void TimeWheel::addTimer(Timer::sPtr timer) {
        SpinLockGuard guard(m_lock);
        m_timers.push_back(timer);
    }

    void TimeWheel::hold() {
        m_last += std::chrono::milliseconds(m_interval);
        std::this_thread::sleep_until(m_last);
    }

    void TimeWheel::add(Timer::sPtr timer) {
        if (timer->delay < m_wheelSize * m_interval) {
            int index = (m_cur + timer->delay / m_interval) % m_wheelSize;
            m_wheel[index].push_back(timer);
            timer->level = 0;
        } else {
            int index = (m_cur + m_wheelSize - 1) % m_wheelSize;
            m_wheel[index].push_back(timer);
            timer->level = timer->delay / (m_wheelSize * m_interval);
        }
    }

    void TimeWheel::tick() {
        {
            SpinLockGuard guard(m_lock);
            for (auto& timer : m_timers) {
                add(timer);
            }
            m_timers.clear();
        }
        m_cur = (m_cur + 1) % m_wheelSize;
        TimerList& list = m_wheel[m_cur];
        for (auto it = list.begin(); it != list.end();) {
            Timer::sPtr timer = *it;
            if (timer->level > 0) {
                timer->level--;
                it++;
                continue;
            }
            if (timer->isRepeat()) {
                timer->delay = timer->interval;
                add(timer);
            }
            it = list.erase(it);
        }
    }
        
}