#pragma once

#include <atomic>
#include <list>
#include "fdContext.h"
#include "socket.h"
#include "packle.h"
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
        SocketContext(int fd = -1, uint32_t serv = 0);
        ~SocketContext();
        bool isValid() const override {
            return m_sock.isValid() && m_serv != 0;
        }
        void handleEvent() override;
        void reset(int fd, uint32_t serv);
        bool close(uint16_t session);
        SocketType sockType() const { return m_sockType; }
        void setSockType(SocketType type) { m_sockType = type; }
        Socket& sock() { return m_sock; }
        void setServ(uint32_t serv) { m_serv = serv; }
        uint32_t serv() const { return m_serv; }
        bool isClosing() const { return m_closing; }
        void setCloseFlag() { m_closing = true; }
        size_t appendWB(char *buf, size_t len);
    private:
        int writeWB();
        int newConnection();
        int handleRead();
        int handleWrite();
        bool shutRead();
        void freeWB();
        bool shutWrite();
        void sendClosedPackle(uint16_t session);
        Socket m_sock;
        uint32_t m_serv;
        int m_readSize;
        SocketType m_sockType;
        bool m_closing;
        uint16_t m_closeSession;
        std::list<WriteBuffer> m_wbList;
    };
}   // Net
}   // Oimo