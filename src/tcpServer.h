#pragma once

#include <functional>
#include <string>

namespace Oimo {
namespace Net {
    class Service;
    class TcpServer {
    public:
        TcpServer(Oimo::Service* serv);
        ~TcpServer();
        void init();
        int initFd(const std::string& ip, uint16_t port);
        void start();
    private:
        void handleNewConn();
        void handleListenReady();
        int m_listenFd;
        Oimo::Service* m_serv;
    };
} // Net
}  // Oimo