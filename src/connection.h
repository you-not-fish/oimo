#pragma once

#include <memory>
#include "ringBuffer.h"
#include "address.h"

namespace Oimo {
namespace Net {
    class Connection {
    public:
        using sPtr = std::shared_ptr<Connection>;
        Connection(int fd, uint32_t ip, uint16_t port)
            : m_fd(fd)
            , m_closing(false)
            , m_addr(ip, port){
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
        size_t recv(char* data, size_t len);
    private:
        int m_fd;
        bool m_closing;
        Address m_addr;
        RingBuffer m_buffer;
    };
} // Net
}  // Oimo