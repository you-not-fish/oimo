#include <regex>
#include <arpa/inet.h>
#include <netdb.h>
#include "address.h"

namespace Oimo {
namespace Net {
    Address::Address() {
        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = INADDR_ANY;
        m_addr.sin_port = 0;
    }


    Address::Address(uint16_t port) {
        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = INADDR_ANY;
        m_addr.sin_port = htons(port);
    }

    Address::Address(const std::string& ip, uint16_t port) {
        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
        m_addr.sin_port = htons(port);
    }

    Address::Address(uint32_t ip, uint16_t port) {
        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = ip;
        m_addr.sin_port = port;
    }

    Address::Address(const struct sockaddr_in& address) {
        m_addr = address;
    }

    Address::~Address() {
    }

    bool Address::setIp(const std::string& ip) {
        m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
        return m_addr.sin_addr.s_addr != INADDR_NONE;
    }

    bool Address::setIp(uint32_t ip) {
        m_addr.sin_addr.s_addr = ip;
        return true;
    }

    bool Address::setPort(const std::string& port) {
        m_addr.sin_port = htons(std::stoi(port));
        return true;
    }

    bool Address::setPort(uint16_t port) {
        m_addr.sin_port = htons(port);
        return true;
    }

    bool Address::setAddr(const struct sockaddr_in& addr) {
        m_addr = addr;
        return true;
    }

    std::string Address::ipAsString() const {
        return std::string(inet_ntoa(m_addr.sin_addr));
    }

    uint32_t Address::ipForNet() const {
        return m_addr.sin_addr.s_addr;
    }

    uint32_t Address::ipForHost() const {
        return ntohl(m_addr.sin_addr.s_addr);
    }

    std::string Address::portAsString() const {
        return std::to_string(ntohs(m_addr.sin_port));
    }

    uint16_t Address::portForNet() const {
        return m_addr.sin_port;
    }

    uint16_t Address::portForHost() const {
        return ntohs(m_addr.sin_port);
    }

    std::string Address::ipPortAsString() const {
        return ipAsString() + ":" + portAsString();
    }

    sockaddr_in Address::sockAddr() const {
        return m_addr;
    }

    bool Address::isIpAddr(const std::string& addr) {
        //match ip or ip:port
        std::regex ipRegex("^(\\d{1,3}\\.){3}\\d{1,3}(:\\d+)?$");
        return std::regex_match(addr, ipRegex);
    }

    bool Address::resolve(const std::string& host, Address& result) {
        // host is ip:port
        if (isIpAddr(host)) {
            std::string ip = host;
            std::string port = "0";
            auto pos = host.find(':');
            if (pos != std::string::npos) {
                ip = host.substr(0, pos);
                port = host.substr(pos + 1);
            }
            result.setIp(ip);
            result.setPort(port);
            return true;
        } else {
            // host is domain name
            struct addrinfo hints;
            struct addrinfo* res;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            hints.ai_flags = AI_PASSIVE;
            int ret = getaddrinfo(host.c_str(), NULL, &hints, &res);
            if (ret != 0) {
                return false;
            }
            result.setAddr(*((struct sockaddr_in*)res->ai_addr));
            freeaddrinfo(res);
            return true;
        }
    }
}   // Net
}   // Oimo