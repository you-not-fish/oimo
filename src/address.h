#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

namespace Oimo {
namespace Net {
    class Address {
    public:
        Address();
        Address(uint16_t port);
        Address(const std::string& ip, uint16_t port);
        Address(uint32_t ip, uint16_t port);
        Address(const struct sockaddr_in& address);
        ~Address();

        bool setIp(const std::string& ip);
        bool setIp(uint32_t ip);
        bool setPort(const std::string& port);
        bool setPort(uint16_t port);
        bool setAddr(const struct sockaddr_in& addr);

        std::string ipAsString() const;
        uint32_t ipForNet() const;
        uint32_t ipForHost() const;
        std::string portAsString() const;
        uint16_t portForNet() const;
        uint16_t portForHost() const;
        std::string ipPortAsString() const;
        sockaddr_in sockAddr() const;

        static bool isIpAddr(const std::string& addr);
        static bool resolve(const std::string& host, Address& result);

    private:
        struct sockaddr_in m_addr;
    };
} // Net
} // Oimo