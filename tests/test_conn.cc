#include <src/application.h>
#include <src/service.h>
#include <src/singleton.h>
#include <src/tcpServer.h>

using namespace Oimo;

class EchoServer : public Service {
public:
    void init(Packle::sPtr packle) override {
        m_server.init(this);
        m_server.createFd("0.0.0.0", 9527);
        m_server.start(std::bind(&EchoServer::onConnect, this, std::placeholders::_1));

    }

    void onConnect(Net::Connection::sPtr conn) {
        auto addr = conn->addr();
        LOG_INFO << "new connection from " << addr.ipAsString() << ":" << addr.portAsString();
        conn->start();
        char buf[1024];
        int cnt = 5;
        while (--cnt) {
            int n = conn->recv(buf, 10);
            if (n == 0) {
                LOG_INFO << "connection closed by peer";
                break;
            }
            if (n > 0) {
                LOG_INFO << "recv: " << std::string(buf, n);
                conn->send(buf, n);
            }
        }
        LOG_INFO << "connection closing...";
        conn->close();
    }
    private:
        Net::TcpServer m_server;
};

class NumService : public Service {
public:
    void init(Packle::sPtr packle) override {
        m_server.init(this);
        m_server.createFd("0.0.0.0", 9528);
        m_server.start(std::bind(&NumService::onConnect, this, std::placeholders::_1));
    }

    void onConnect(Net::Connection::sPtr conn) {
        auto addr = conn->addr();
        LOG_INFO << "new connection from " << addr.ipAsString() << ":" << addr.portAsString();
        conn->start();
        int cnt = 1000;
        while (cnt--) {
            std::string s = std::to_string(cnt);
            conn->send(s.c_str(), s.size());
            ::sleep(1);
        }
        LOG_INFO << "connection closing...";
        conn->close();
    }
private:
    Net::TcpServer m_server;
};

int main() {
    auto& app = Singleton<Application>::instance();
    app.init();
    Logger::setLogLevel(LogLevel::TRACE);
    app.newService<EchoServer>("EchoServer");
    app.newService<NumService>("NumService");
    return app.run();
}

