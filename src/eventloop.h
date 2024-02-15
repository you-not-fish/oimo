#pragma once

#include <vector>
#include "epoller.h"

namespace Oimo {
namespace Net {
    class FdContext;
    class EventLoop {
    public:
        EventLoop();
        ~EventLoop();
        void loop();
        void stop() { m_looping = false; }
        void updateEvent(FdContext* ctx);
    private:
        EPoller m_poller;
        bool m_looping;
        std::vector<FdContext*> m_activeFds;
    };
}   // Net
}   // Oimo