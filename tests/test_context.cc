#include <iostream>
#include <cassert>
#include <src/application.h>
#include <src/queue.h>
#include <src/sysMsg.h>
#include <src/thread.h>
#include <src/singleton.h>
#include "protobuf/response.pb.h"

using namespace Oimo;

enum class MsgID : Packle::MsgID {
    Query = 1,
    Response
};

class QueryService : public Service {
public:
    void init(Packle::sPtr packle) override {
        assert(context() == ServiceContext::currentContext());
        Packle::sPtr query = std::make_shared<Packle>((Packle::MsgID)MsgID::Query);
        call("DBService", query);
        Packle::sPtr response = responsePackle();
        test::Response resp = response->deserialize<test::Response>();
        LOG_INFO << "name: " << resp.name() << " age: " << resp.age() << " email: " << resp.email(0);
    }
};

class DBService : public Service{
public:
    void init(Packle::sPtr packle) override {
        assert(context() == ServiceContext::currentContext());
        registerFunc((Packle::MsgID)MsgID::Query,
            std::bind(&DBService::query, this, std::placeholders::_1));
    }

    void query(Packle::sPtr packle) {
        Packle::sPtr response = std::make_shared<Packle>((Packle::MsgID)MsgID::Response);
        test::Response resp;
        resp.set_name("nagisa");
        resp.set_age(18);
        resp.add_email("xxx@xxx.com");
        response->serialize(resp);
        response->setSessionID(packle->sessionID());
        setReturnPackle(response);
    }
};

// void worker() {
//     auto& G = Singleton<GlobalQueue>::instance();
//     while(true) {
//         // LOG_INFO << "Global Queue Size: " << G.size();
//         if (!G.empty()) {
//             PackleQueue::sPtr queue = G.pop();
//             std::deque<Packle::sPtr> que;
//             auto context = queue->context();
//             queue->swap(que);
//             LOG_INFO << "process message from : " << context->name();
//             assert(context);
//             ServiceContext::setCurrentContext(context);
//             context->doFork();
//             while (!que.empty()) {
//                 Packle::sPtr packle = que.front();
//                 que.pop_front();
//                 assert(packle);
//                 context->dispatch(packle);
//             }
//             ServiceContext::setCurrentContext(nullptr);
//         }
//     }
// }

int main() {
    Application app;
    app.init();
    app.newService<DBService>("DBService");
    app.newService<QueryService>("QueryService");
    // Thread::sPtr t1 = std::make_shared<Thread>(worker, "worker1");
    // Thread::sPtr t2 = std::make_shared<Thread>(worker, "worker2");
    // t1->start();
    // t2->start();
    // t1->join();
    // t2->join();
    return app.run();
}