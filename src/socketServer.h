#pragma once

#include "singleton.h"
#include "socketContext.h"
#include "pipeContext.h"
#include "eventloop.h"

namespace Oimo {
namespace Net {
    #define MAX_SOCKETS 1<<16
    class SocketServer {
    public:
        SocketServer();
        EventLoop* eventLoop() { return &m_loop; }
        ssize_t sendCtrl(const char *data, size_t len) {
            return m_pipeCtx.writePipe(data, len);
        }
        SocketContext* getSocketContext(int fd) { return &m_contexts[fd]; }
    private:
        SocketContext m_contexts[MAX_SOCKETS];
        PipeContext m_pipeCtx;
        EventLoop m_loop;
    };
    using GSocketServer = Singleton<SocketServer>;
}  // Net
}  // Oimo