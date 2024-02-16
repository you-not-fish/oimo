#pragma once

#include <functional>
#include <string>
#include <map>
#include <functional>
#include "connection.h"

namespace Oimo {
namespace Net {
    class Service;
    class TcpServer {
    public:
        using ConnCb = std::function<void(Connection::sPtr)>;
        TcpServer();
        ~TcpServer();
        void init(Oimo::Service* serv);
        int createFd(const std::string& ip, uint16_t port);
        void start(ConnCb cb);
    private:
        void handleNewConn(Oimo::Packle::sPtr packle);
        int m_listenFd;
        ConnCb m_cb;
        Oimo::Service* m_serv;
        std::map<int, Connection::sPtr> m_conns;
    };
} // Net
}  // Oimo