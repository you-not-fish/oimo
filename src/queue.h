#pragma once

#include <memory>
#include <queue>
#include "serviceContext.h"
#include "packle.h"
#include "spinLock.h"

namespace Oimo {
    class PackleQueue
        : public std::enable_shared_from_this<PackleQueue>{
    public:
        using sPtr = std::shared_ptr<PackleQueue>;
        PackleQueue(ServiceContext::sPtr context = nullptr)
            : m_context(context)
            , m_isInGlobal(false) {
        }
        ~PackleQueue() = default;
        void push(Packle::sPtr packle);
        Packle::sPtr pop();
        bool empty() {
            SpinLockGuard guard(m_lock);
            return m_queue.empty();
        }
        ServiceContext::sPtr context() const {
            return m_context;
        }
        bool isInGlobal() const {
            return m_isInGlobal;
        }
        void setInGlobal(bool isInGlobal) {
            m_isInGlobal = isInGlobal;
        }
        void setContext(ServiceContext::sPtr context) {
            m_context = context;
        }
        void swap(PackleQueue& other) {
            SpinLockGuard guard(m_lock);
            other.m_queue.swap(m_queue);
            setInGlobal(false);
        }
    private:
        ServiceContext::sPtr m_context;
        bool m_isInGlobal;
        std::deque<Packle::sPtr> m_queue;
        SpinLock m_lock;
    };

    class GlobalQueue {
    public:
        using sPtr = std::shared_ptr<GlobalQueue>;
        GlobalQueue() = default;
        ~GlobalQueue() = default;
        void push(PackleQueue::sPtr packleQueue) {
            SpinLockGuard guard(m_lock);
            m_queue.push_back(packleQueue);
        }
        PackleQueue::sPtr pop() {
            SpinLockGuard guard(m_lock);
            if (m_queue.empty()) {
                return nullptr;
            }
            auto packleQueue = m_queue.front();
            m_queue.pop_front();
            return packleQueue;
        }
        bool empty() {
            SpinLockGuard guard(m_lock);
            return m_queue.empty();
        }
    private:
        std::deque<PackleQueue::sPtr> m_queue;
        SpinLock m_lock;
    };
}