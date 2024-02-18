#pragma once

#include <functional>
#include <string>
#include <map>
#include <functional>
#include "connection.h"

namespace Oimo {
    class Service;
namespace Net {
    class TcpServer {
    public:
        using ConnCb = std::function<void(Connection::sPtr)>;
        TcpServer();
        ~TcpServer();
        void init(Oimo::Service* serv);
        int createFd(const std::string& ip, uint16_t port);
        void start(ConnCb cb);
        void addConn(int fd, Connection::sPtr conn);
        void removeConn(int fd);
    private:
        void handleNewConn(Oimo::Packle::sPtr packle);
        void handleRead(Oimo::Packle::sPtr packle);
        void handleError(Oimo::Packle::sPtr packle);
        void handleHalfClose(Oimo::Packle::sPtr packle);
        int m_listenFd;
        ConnCb m_cb;
        Oimo::Service* m_serv;
        std::map<int, Connection::sPtr> m_conns;
    };
} // Net
}  // Oimo