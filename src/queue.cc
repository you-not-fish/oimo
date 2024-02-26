#include "queue.h"
#include "serviceContext.h"
#include "singleton.h"

namespace Oimo {
    void PackleQueue::push(Packle::sPtr packle) {
        SpinLockGuard guard(m_lock);
        m_queue.push_back(packle);
        if (!m_isInGlobal && !m_isProcessing) {
            Singleton<GlobalQueue>::instance().push(shared_from_this());
        }
    }

    Packle::sPtr PackleQueue::pop() {
        SpinLockGuard guard(m_lock);
        if (m_queue.empty()) {
            return nullptr;
        }
        auto packle = m_queue.front();
        m_queue.pop_front();
        return packle;
    }

    void PackleQueue::startProcess() {
        m_isProcessing = true;
        m_isInGlobal = false;
    }

    void PackleQueue::finishProcess(bool isForked) {
        SpinLockGuard guard(m_lock);
        m_isProcessing = false;
        if (isForked || !m_queue.empty()) {
            Singleton<GlobalQueue>::instance().push(shared_from_this());
        }
    }
} // namespace Oimo
