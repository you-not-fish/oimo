#include <cassert>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include "socket.h"
#include "fcntl.h"
#include "log.h"
#include "address.h"

namespace Oimo {
namespace Net {
    Socket::~Socket() {
        close();
    }
    bool Socket::create(int domain, int type, int protocol) {
        if (m_fd != -1) {
            LOG_WARN << "Socket already created";
            close();
        }
        m_fd = ::socket(domain, type, protocol);
        return m_fd != -1;
    }

    bool Socket::bind(const Address& address) {
        assert(m_fd != -1);
        auto addr = address.sockAddr();
        if (::bind(m_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            LOG_FATAL << "bind failed : " << strerror(errno);
            return false;
        }
        return  true;
    }

    bool Socket::listen() {
        assert(m_fd != -1);
        if (::listen(m_fd, SOMAXCONN) < 0) {
            LOG_FATAL << "listen failed : " << strerror(errno);
            return false;
        }
        return true;
    }

    int Socket::accept(Address* peerAddr) {
        struct sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);
        int fd = ::accept(m_fd, (struct sockaddr*)&addr, &addrLen);
        if (fd != -1) {
            if (peerAddr) {
                peerAddr->setAddr(addr);
            }
            return fd;
        }
        LOG_FATAL << "accept failed : " << strerror(errno);
        return -1;
    }

    bool Socket::close() {
        if (m_fd != -1) {
            ::close(m_fd);
            m_fd = -1;
            return true;
        }
        return false;
    }

    bool Socket::setNonBlocking(bool nonBlocking) {
        int flags = ::fcntl(m_fd, F_GETFL, 0);
        if (flags == -1) {
            LOG_ERROR << "fcntl failed : " << strerror(errno);
            return false;
        }
        if (nonBlocking) {
            flags |= O_NONBLOCK;
        } else {
            flags &= ~O_NONBLOCK;
        }
        return ::fcntl(m_fd, F_SETFL, flags) != -1;
    }

    bool Socket::setReuseAddr(bool reuseAddr) {
        int opt = reuseAddr ? 1 : 0;
        return ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != -1;
    }

    bool Socket::setReusePort(bool reusePort) {
        int opt = reusePort ? 1 : 0;
        return ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) != -1;
    }

    bool Socket::setKeepAlive(bool keepAlive) {
        int opt = keepAlive ? 1 : 0;
        return ::setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) != -1;
    }

    bool Socket::setNoDelay(bool noDelay) {
        int opt = noDelay ? 1 : 0;
        return ::setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) != -1;
    }
} // namespace Net
} // namespace Oimo