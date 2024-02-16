#include <cassert>
#include <unistd.h>
#include "epoller.h"
#include "socketContext.h"
#include "log.h"

namespace Oimo {
namespace Net {
    EPoller::EPoller() : m_events(MIN_EVENTS) {
        m_epfd = ::epoll_create1(EPOLL_CLOEXEC);
        if (m_epfd < 0) {
            LOG_FATAL << "epoll_create1 failed";
        }
    }

    EPoller::~EPoller() {
        ::close(m_epfd);
    }

    void EPoller::poll(std::vector<FdContext*>& activeFds) {
        int numEvents = ::epoll_wait(m_epfd, m_events.data(), m_events.size(), -1);
        if (numEvents < 0) {
            LOG_ERROR << "epoll_wait failed : " << strerror(errno);
        }
        for (int i = 0; i < numEvents; ++i) {
            FdContext* ctx = static_cast<FdContext*>(m_events[i].data.ptr);
            ctx->setRevents(m_events[i].events);
            activeFds.push_back(ctx);
        }
        if (numEvents == static_cast<int>(m_events.size())) {
            int size = m_events.size() * 2 > MAX_EVENTS
                ? MAX_EVENTS : m_events.size() * 2;
            m_events.resize(size);
        }
    }

    void EPoller::updateEvent(FdContext* ctx) {
        if (ctx->type() == EventType::NEW) {
            update(EPOLL_CTL_ADD, ctx);
            ctx->setType(EventType::ADDED);
        } else {
            if (ctx->isNoneEvent()) {
                update(EPOLL_CTL_DEL, ctx);
                ctx->setType(EventType::NEW);
            } else {
                update(EPOLL_CTL_MOD, ctx);
            }
        }
    }

    void EPoller::update(int operation, FdContext* ctx) {
        struct epoll_event event;
        event.events = ctx->events();
        event.data.ptr = ctx;
        int ret = ::epoll_ctl(m_epfd, operation, ctx->fd(), &event);
        if (ret < 0) {
            LOG_ERROR << "epoll_ctl failed";
        }
    }
}   //namespace Net
}   //namespace Oimo
