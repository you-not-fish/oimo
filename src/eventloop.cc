#include <cassert>
#include "eventloop.h"
#include "socketContext.h"

namespace Oimo {
namespace Net {
    EventLoop::EventLoop() : m_poller(), m_looping(false) {}
    EventLoop::~EventLoop() {}

    void EventLoop::loop() {
        m_looping = true;
        while (m_looping) {
            m_activeFds.clear();
            m_poller.poll(m_activeFds);
            for (auto ctx : m_activeFds) {
                assert(ctx);
                ctx->handleEvent();
            }
        }
    }

    void EventLoop::updateEvent(FdContext* ctx) {
        m_poller.updateEvent(ctx);
    }
}   // Net
}   // Oimo