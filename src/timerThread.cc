#include "timerThread.h"
#include "timeWheel.h"

namespace Oimo {
    TimeThread::TimeThread() {}

    TimeThread::~TimeThread() {
        stop();
        join();
    }

    void TimeThread::start() {
        m_thread.reset(new Thread(std::bind(&TimeThread::run, this), "timethread"));
        m_thread->start();
    }

    void TimeThread::join() {
        m_thread->join();
    }

    void TimeThread::stop() {
        auto& wheel = GTimerWheel::instance();
        wheel.stop();
    }

    void TimeThread::run() {
        auto& wheel = GTimerWheel::instance();
        wheel.run();
    }
}