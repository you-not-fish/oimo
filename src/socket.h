#pragma once


namespace Oimo {
namespace Net{
    class Address;
    class Socket {
    public:
        Socket(int sock = -1) : m_fd(sock) {}
        ~Socket();

        bool create(int domain, int type, int protocol);
        void setFd(int fd) { m_fd = fd; }
        bool bind(const Address& addr);
        bool listen();
        int accept(Address* addr);
        bool close();
        int fd() const { return m_fd; }
        bool isValid() const { return m_fd != -1; }
        bool setNonBlocking(bool nonBlocking);
        bool setReuseAddr(bool reuseAddr);
        bool setReusePort(bool reusePort);
        bool setKeepAlive(bool keepAlive);
        bool setNoDelay(bool noDelay);
    private:
        int m_fd { -1 };
    };
} // namespace Net
} // namespace Oimo
