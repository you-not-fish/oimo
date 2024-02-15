#include "socketState.h"
#include "eventloop.h"
#include "socketContext.h"

namespace Oimo {
namespace Net {

    SocketContext::SocketContext(EventLoop* loop, int fd, uint32_t serv)
        : FdContext(loop, fd)
        , m_sock(fd)
        , m_serv(serv)
        , m_sockType(SocketType::NEW)
        , m_sending(0) {}

    SocketContext::~SocketContext() {}
}  // Net
} // Oimo