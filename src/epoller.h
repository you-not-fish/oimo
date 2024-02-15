#pragma once

#include <sys/epoll.h>
#include <vector>

namespace Oimo {
namespace Net {
    #define MIN_EVENTS 64
    #define MAX_EVENTS 256
    class FdContext;
    class EPoller {
    public:
        EPoller();
        ~EPoller();
        void poll(std::vector<FdContext*>& activeFds);
        void updateEvent(FdContext* ctx);
    private:
        void update(int operation, FdContext* ctx);
        int m_epfd;
        std::vector<struct epoll_event> m_events;
    };
} // namespace Net
} // namespace Oimo