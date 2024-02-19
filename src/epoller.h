#pragma once

#include <sys/epoll.h>
#include <string>
#include <vector>

namespace Oimo {
namespace Net {
    #define MIN_EVENTS 64
    #define MAX_EVENTS 256

    inline const char* epollopToStr(int op) {
        switch (op) {
            case EPOLL_CTL_ADD:
                return "EPOLL_CTL_ADD";
            case EPOLL_CTL_MOD:
                return "EPOLL_CTL_MOD";
            case EPOLL_CTL_DEL:
                return "EPOLL_CTL_DEL";
            default:
                return "UNKNOWN";
        }
    }

    inline std::string eventsToStr(int events) {
        std::string str;
        if (events & EPOLLIN) {
            str += "EPOLLIN ";
        }
        if (events & EPOLLOUT) {
            str += "EPOLLOUT ";
        }
        if (events & EPOLLRDHUP) {
            str += "EPOLLRDHUP ";
        }
        if (events & EPOLLHUP) {
            str += "EPOLLHUP ";
        }
        if (events & EPOLLERR) {
            str += "EPOLLERR ";
        }
        return str;
    }

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