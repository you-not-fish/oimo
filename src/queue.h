#pragma once

#include <memory>
#include <queue>
#include "packle.h"
#include "spinLock.h"
#include "singleton.h"

namespace Oimo {
    class ServiceContext;
    class PackleQueue
        : public std::enable_shared_from_this<PackleQueue>{
    public:
        using sPtr = std::shared_ptr<PackleQueue>;
        PackleQueue(std::shared_ptr<ServiceContext> context = nullptr)
            : m_context(context)
            , m_isInGlobal(false)
            , m_isProcessing(false) {
        }
        ~PackleQueue() = default;
        void push(Packle::sPtr packle);
        Packle::sPtr pop();
        bool empty() {
            SpinLockGuard guard(m_lock);
            return m_queue.empty();
        }
        size_t size() {
            SpinLockGuard guard(m_lock);
            return m_queue.size();
        }
        std::weak_ptr<ServiceContext> context() const {
            return m_context;
        }
        bool isInGlobal() const {
            return m_isInGlobal;
        }
        void setInGlobal(bool isInGlobal) {
            m_isInGlobal = isInGlobal;
        }
        bool isProcessing() const {
            return m_isProcessing;
        }
        void setProcessing(bool isProcessing) {
            m_isProcessing = isProcessing;
        }
        void setContext(std::shared_ptr<ServiceContext> context) {
            m_context = context;
        }
        void startProcess();
        void finishProcess(bool isForked);
    private:
        std::weak_ptr<ServiceContext> m_context;
        bool m_isInGlobal;
        bool m_isProcessing;
        std::deque<Packle::sPtr> m_queue;
        SpinLock m_lock;
    };

    class GlobalQueue {
    public:
        using sPtr = std::shared_ptr<GlobalQueue>;
        GlobalQueue() = default;
        ~GlobalQueue() = default;
        void push(PackleQueue::sPtr que) {
            SpinLockGuard guard(m_lock);
            m_queue.push_back(que);
            que->setInGlobal(true);
        }
        PackleQueue::sPtr pop() {
            SpinLockGuard guard(m_lock);
            if (m_queue.empty()) {
                return nullptr;
            }
            auto que = m_queue.front();
            m_queue.pop_front();
            return que;
        }
        bool empty() {
            SpinLockGuard guard(m_lock);
            return m_queue.empty();
        }
        size_t size() {
            // SpinLockGuard guard(m_lock);
            return m_queue.size();
        }
    private:
        std::deque<PackleQueue::sPtr> m_queue;
        SpinLock m_lock;
    };

    using GQueue = Singleton<GlobalQueue>;
}