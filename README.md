# Oimo（更新中）
## 简介
练手项目（更新到网络模块）
高性能游戏服务器，也可作为web服务器，总体设计参考skynet的actor模式.基于此框架的游戏demo：[https://github.com/you-not-fish/oimo_demo](https://github.com/you-not-fish/oimo_demo)

## 技术要点
1. **日志模块**：参考muoduo库，实现异步可回滚日志系统，日志前后端分离，临界区小且线程安全；
2. **配置模块**：采用yaml格式进行配置，约定优于配置，配置文件路径可通过环境变量OIMO_CONFIG指定（export OIMO_CONFIG=/path/to/config.yaml），不指定则默认从当前目录寻找。
3. **线程模块**；多线程，包括日志线程、定时器线程，网络线程和若干工作线程，工作线程负责协程调度；
4. **协程模块**：参考腾讯libco库，通过嵌入汇编切换寄存器上下文；
5. **服务模块**：Actor实例，可通过消息队列进行消息收发和服务间同步rpc调用（由于采用协程实现，所以底层是异步的不会阻塞线程，只是对业务层透明）；
6. **定时器模块**： 基于TimeWheel算法实现；
7. **网络模块**： 基于reactor模型实现，内部使用epoll做事件监听；

## 依赖
1. yaml-cpp：读取配置文件；
2. protobuf（可选）：测试文件中用到，框架本身不需要。

## 主要API
1. **Application::init**: 初始化框架；
2. **Application::run**: 运行框架；
3. **Appilcation::newService**: 创建一个新服务；
4. **Service::send**: 向其他服务发送消息；
5. **Service::call**: 服务间rpc调用，与send的区别是，当调用call时，发出消息后，框架会立即挂起当前协程，直到收到响应后再唤醒该协程；
6. **Service::responsePackle**: 调用call之后获取返回结果；
7. **Service::setReturnPackle**: 消息处理结束后设置返回值；
8. **Service::addTimer**: 添加定时器；
9. **Service::removeTimer**: 删除定时器；

## 使用说明
下面简单介绍oimo框架使用方法，详细用法可以参考简介中提到的游戏demo.
### 注意事项
1. 此项目要求C++17标准（项目中使用了C++17的std::any）；
1. 框架中所有服务的回调都是在协程中执行，协程对业务层是透明的，如果用户自己创建协程，可以使用Service::fork，不推荐直接使用Coroutine类创建协程，因为这样的协程不会纳入框架内部管理；
2. 框架内部机制保证同一时刻同一服务内的逻辑只被一个线程执行，所以不用担心线程安全问题，业务层代码无需加锁；
3. 框架不限定网络消息的序列化方式，用户可以自己选择json、google protobuf等方式，不过如果选择google protobuf的话，可以直接使用框架内提供的Packle::serialize和Packle::deserialize进行序列化和反序列化；
4. 框架没有hook任何系统函数，但常用的阻塞函数框架都做了异步封装（如sleep、send、recv等等），用户依然可以用写同步程序的方法写出异步程序；
### 使用步骤
1. 创建服务类，如EchoService，该类必须继承Oimo::Service类；
2. 实现init虚方法，init方法会在服务启动时执行，可以在init方法中通过registerFunc注册回调，初始化网络连接等；
3. 如果要进行网络监听，服务类中需要包含Oimo::Net::TcpServer成员，并在init方法中初始化；
4. 实现服务类中的回调方法；
5. 在main方法中用Application::init初始化框架，用Application::newService创建服务，用Application::run启动框架；
### 使用示例
下面这个例子实现了一个简单功能，用户输入名字，查询自己的成绩.
示例中有两个服务，QueryService接受网络数据，向DBService发起查询请求，并将查询结果返回给用户.

```cpp
#include <application.h>
#include <tcpServer.h>

using namespace Oimo;

enum class MsgType {
    Query,
};

class QueryService : public Service {
public:
    void init(Packle::sPtr packle) {
        // 初始或tcpserver
        m_serv.init(this);
        // 初始化监听套接字
        m_serv.createFd("0.0.0.0", 9527);
        // 启动监听，当连接到来时会调用onConnect方法
        m_serv.start(
            std::bind(&QueryService::onConnect, this, std::placeholders::_1)
        );
        // 120s后结束程序
        addTimer(120000, 0 , []{
            APP::instance().stop();
        });
    }
private:
    void onConnect(Net::Connection::sPtr conn) {
        // 开启连接
        conn->start();
        auto addr = conn->addr();
        LOG_INFO << "new connection from "
            << addr.ipAsString() << ":" << addr.portAsString();
        char data[1024];
        while (true) {
            // 接收数据
            int n = conn->recv(data, 1024);
            if (n == 0) break;
            std::string str(data, n - 2); // 去掉\r\n
            LOG_INFO << "recv: " << str;
            // 向DBService查询
            Packle::sPtr packle = std::make_shared<Packle>(
                (Packle::MsgID)MsgType::Query
            );
            packle->userData = str;
            // 发送数据并挂起
            call("db", packle);
            // 获取返回数据
            auto resp = responsePackle();
            auto ret = std::any_cast<std::string>(resp->userData);
            // 发送返回数据
            conn->send(ret.c_str(), ret.size());
        }
        conn->close();
    }
private:
    Net::TcpServer m_serv;
};

class DBService : public Service {
public:
    void init(Packle::sPtr packle) {
        // 注册回调
        registerFunc((Packle::MsgID)MsgType::Query,
            std::bind(&DBService::onQuery, this, std::placeholders::_1));
        // 初始化数据
        m_data["yui"] = 70;
        m_data["mio"] = 68;
        m_data["ritsu"] = 95;
        m_data["tsumugi"] = 100;
    }
private:
    void onQuery(Packle::sPtr packle) {
        // 模拟查询
        std::string query = std::any_cast<std::string>(packle->userData);
        std::string ret = "unknown\n";
        auto it = m_data.find(query);
        if (it != m_data.end()) {
            ret = std::to_string(it->second) + "\n";
        }
        packle->userData = ret;
        // 返回数据
        setReturnPackle(packle);
    }
    std::map<std::string, int> m_data;
};

int main() {
    auto& app = APP::instance();
    app.init();
    // 启动服务
    app.newService<DBService>("db");
    app.newService<QueryService>("query");
    // 运行
    return app.run();
}
```

## 参考
1. https://github.com/cloudwu/skynet
2. https://github.com/chenshuo/muduo
3. https://github.com/Tencent/libco
4. https://github.com/sylar-yin/sylar
5. https://github.com/Gooddbird/tinyrpc