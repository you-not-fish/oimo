#pragma once

#include <vector>
#include "packle.h"
#include "ringBuffer.h"
#include "address.h"
#include "coroutine.h"

namespace Oimo {
namespace Net {
    class TcpServer;
    class Connection {
    public:
        using sPtr = std::shared_ptr<Connection>;
        Connection(int fd, uint32_t ip, uint16_t port, TcpServer* serv)
            : m_fd(fd)
            , m_closing(false)
            , m_addr(ip, port)
            , m_serv(serv) {
        }
        ~Connection() = default;
        int fd() const {
            return m_fd;
        }
        Address addr() const {
            return m_addr;
        }
        void start();
        void close();
        size_t send(const char* data, size_t len);
        size_t send(Oimo::Packle::sPtr packle);
        size_t recv(char* data, size_t len);
        size_t append(const char* data, size_t len);
        void setCloseFlag() {
            m_closing = true;
        }
    private:
        void sendToSocket(const char *buf, size_t len, bool needCopy);
        int m_fd;
        bool m_closing;
        Address m_addr;
        RingBuffer m_buffer;
        std::vector<Oimo::Coroutine::sPtr> m_cors;
        TcpServer* m_serv;
    };
} // Net
}  // Oimo