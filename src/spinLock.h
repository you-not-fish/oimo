#pragma once

#include <atomic>
#include "noncopyable.h"

namespace Oimo {
    class SpinLock : noncopyable {
    public:
        void lock() {
            while (m_lock.test_and_set(std::memory_order_acquire)) {
            }
        }
        void unlock() {
            m_lock.clear(std::memory_order_release);
        }
    private:
        std::atomic_flag m_lock = ATOMIC_FLAG_INIT;
    };

    class SpinLockGuard : noncopyable {
    public:
        SpinLockGuard(SpinLock& lock)
            : m_lock(lock) {
            m_lock.lock();
        }
        ~SpinLockGuard() {
            m_lock.unlock();
        }
    private:
        SpinLock& m_lock;
    };
} // namespace Oimo