#pragma once

#include <atomic>
#include <list>
#include "fdContext.h"
#include "socket.h"
#include "socketState.h"

namespace Oimo {
namespace Net {
    
    struct WriteBuffer {
        char* buf;
        size_t size;
        size_t pos;
    };
    class SocketContext : public FdContext {
    public:
        SocketContext(EventLoop* loop = nullptr, int fd = -1, uint32_t serv = 0);
        ~SocketContext();
        void handleEvent() override;
        SocketType sockType() const { return m_sockType; }
    private:
        Socket m_sock;
        uint32_t m_serv;
        SocketType m_sockType;
        std::atomic_int m_sending;
        std::list<WriteBuffer> m_wbList;
    };
}   // Net
}   // Oimo