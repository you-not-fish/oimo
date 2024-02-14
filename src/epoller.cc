#include <cassert>
#include <unistd.h>
#include "epoller.h"
#include "log.h"

namespace Oimo {
namespace Net {
    EPoller::EPoller() {
        m_epfd = ::epoll_create1(EPOLL_CLOEXEC);
        if (m_epfd < 0) {
            LOG_FATAL << "epoll_create1 failed";
        }
    }

    EPoller::~EPoller() {
        ::close(m_epfd);
    }

    bool EPoller::add(int fd, epoll_event* event) {
        return ::epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, event) == 0;
    }

    bool EPoller::mod(int fd, epoll_event* event) {
        return ::epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, event) == 0;
    }

    bool EPoller::del(int fd) {
        return ::epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, nullptr) == 0;
    }

    int EPoller::wait(epoll_event* events, int maxEvents, int timeout) {
        return ::epoll_wait(m_epfd, events, maxEvents, timeout);
    }
}   //namespace Net
}   //namespace Oimo
