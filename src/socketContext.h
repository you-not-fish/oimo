#pragma once

#include <atomic>
#include <list>
#include "fdContext.h"
#include "socket.h"
#include "packle.h"
#include "socketState.h"
#include "spinLock.h"

namespace Oimo {
namespace Net {
    
    struct WriteBuffer {
        char* buf;
        size_t size;
        size_t pos;
    };
    class SocketContext : public FdContext {
    public:
        SocketContext(int fd = -1, uint32_t serv = 0);
        ~SocketContext();
        bool isValid() const {
            return m_sock.isValid() && m_serv != 0;
        }
        void handleEvent() override;
        void reset(int fd, uint32_t serv);
        void close() { m_sock.close(); m_fd = -1; }
        SocketType sockType() const { return m_sockType; }
        void setSockType(SocketType type) { m_sockType = type; }
        Socket& sock() { return m_sock; }
        void setServ(uint32_t serv) { m_serv = serv; }
        uint32_t serv() const { return m_serv; }
        size_t write(const char *buf, size_t len, bool needCopy);
    private:
        bool writeWB();
        void newConnection();
        void handleRead();
        void handleWrite();
        Socket m_sock;
        uint32_t m_serv;
        int m_readSize;
        SocketType m_sockType;
        std::list<WriteBuffer> m_wbList;
        std::list<WriteBuffer> m_wbList0;
        Oimo::SpinLock m_lock;
    };
}   // Net
}   // Oimo