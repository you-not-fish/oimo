#include <sys/epoll.h>
#include "fdContext.h"
#include "eventloop.h"
#include "socketState.h"

namespace Oimo {
namespace Net {
    const int FdContext::kNoneEvent = 0;
    const int FdContext::kReadEvent = EPOLLIN | EPOLLPRI;
    const int FdContext::kWriteEvent = EPOLLOUT;

    FdContext::FdContext(EventLoop* loop, int fd)
        : m_loop(loop)
        , m_fd(fd)
        , m_type(EventType::NEW)
        , m_events(kNoneEvent) {}

    FdContext::~FdContext() {}

    void FdContext::update() {
        m_loop->updateEvent(this);
    }
}  // Net
} // Oimo