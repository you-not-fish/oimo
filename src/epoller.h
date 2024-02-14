#pragma once

#include <sys/epoll.h>

namespace Oimo {
namespace Net {
    class EPoller {
    public:
        EPoller();
        ~EPoller();

        bool add(int fd, epoll_event* event);
        bool mod(int fd, epoll_event* event);
        bool del(int fd);
        int wait(epoll_event* events, int maxEvents, int timeout);
    private:
        int m_epfd;
    };
} // namespace Net
} // namespace Oimo