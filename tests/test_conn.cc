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
        int n = conn->recv(buf, 1024);
        if (n > 0) {
            LOG_INFO << "recv: " << std::string(buf, n);
        }
    }
    private:
        Net::TcpServer m_server;
};

int main() {
    auto& app = Singleton<Application>::instance();
    app.init();
    Logger::setLogLevel(LogLevel::DEBUG);
    app.newService<EchoServer>("EchoServer");
    return app.run();
}

