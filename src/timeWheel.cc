#include <thread>
#include "timeWheel.h"
#include "sysMsg.h"
#include "packle.h"
#include "serviceContextMgr.h"
#include "config.h"
#include "log.h"

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
        // LOG_DEBUG << "m_last: " << std::chrono::duration_cast<std::chrono::milliseconds>(m_last.time_since_epoch()).count();
        // LOG_DEBUG << "now: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        std::this_thread::sleep_until(m_last);
    }

    void TimeWheel::add(Timer::sPtr timer) {
        uint32_t ticks = timer->delay ? (timer->delay - 1) / m_interval + 1 : 1;
        uint32_t level = (ticks - 1) / m_wheelSize;
        int slot = (m_cur + ticks) % m_wheelSize;
        timer->level = level;
        m_wheel[slot].push_back(timer);
        LOG_DEBUG << "add timer, delay: " << timer->delay << ", interval: " << timer->interval
            << ", ticks: " << ticks << ", level: " << level
            << ", cur_index: " << m_cur << ", slot: " << slot;
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
            if (!timer.unique()) {
                execute(timer->serv, timer->session);
                if (timer->isRepeat()) {
                    // LOG_DEBUG << "now : " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
                    timer->delay = timer->interval;
                    add(timer);
                }    
            }
            it = list.erase(it);
        }
    }
    
    void TimeWheel::execute(uint32_t serv, uint16_t session) {
        Packle::sPtr packle = std::make_shared<Packle>((Packle::MsgID)SystemMsgID::TIMER);
        packle->setSessionID(session);
        auto ctx = ServiceContextMgr::getContext(serv);
        ctx->messageQueue()->push(packle);
    }
}