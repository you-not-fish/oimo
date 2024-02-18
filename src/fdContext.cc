#include <cassert>
#include <sys/epoll.h>
#include "fdContext.h"
#include "eventloop.h"
#include "serviceContextMgr.h"
#include "socketServer.h"
#include "socketState.h"

namespace Oimo {
namespace Net {
    const int FdContext::kNoneEvent = 0;
    const int FdContext::kReadEvent = EPOLLIN | EPOLLPRI;
    const int FdContext::kWriteEvent = EPOLLOUT;

    FdContext::FdContext(int fd)
        : m_fd(fd)
        , m_type(EventType::NEW)
        , m_events(kNoneEvent) {}

    FdContext::~FdContext() {}

    void FdContext::reset() {
        m_fd = -1;
        m_type = EventType::NEW;
        m_events = kNoneEvent;
        m_revents = 0;
    }

    void FdContext::update() {
        auto loop = GSocketServer::instance().eventLoop();
        loop->updateEvent(this);
    }

    void FdContext::sendProto(const Oimo::Packle::sPtr& packle, uint32_t serv) {
        auto servCtx = Oimo::ServiceContextMgr::getContext(serv);
        assert(servCtx);
        servCtx->messageQueue()->push(packle);
    }
}  // Net
} // Oimo