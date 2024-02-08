#include "../src/init.h"
#include "../src/log.h"
#include "../src/queue.h"
#include "../src/thread.h"
#include "../src/singleton.h"
#include "../src/serviceContext.h"
#include "protobuf/response.pb.h"

using namespace Oimo;

enum class MsgID : Packle::MsgID {
    Query,
    Response
};

class QueryService {
public:
    QueryService(ServiceContext::sPtr context) : m_context(context) {
        m_context->registerHandler((Packle::MsgID)SystemMsgID::INIT,
            std::bind(&QueryService::init, this, std::placeholders::_1));
    }
    void init(Packle::sPtr packle) {
        LOG_INFO << "QueryService::init";
        Packle::sPtr query = std::make_shared<Packle>((Packle::MsgID)MsgID::Query);
        ServiceContext::call("DBService", query);
        Packle::sPtr response = m_context->responsePackle();
        test::Response resp = response->deserialize<test::Response>();
        LOG_INFO << "name: " << resp.name() << " age: " << resp.age() << " email: " << resp.email(0);

    }
private:
    ServiceContext::sPtr m_context;
};

class DBService {
public:
    DBService(ServiceContext::sPtr context) : m_context(context) {
        m_context->registerHandler((Packle::MsgID)SystemMsgID::INIT,
            std::bind(&DBService::init, this, std::placeholders::_1));
    }

    void init(Packle::sPtr packle) {
        LOG_INFO << "DBService::init";
        m_context->registerHandler((Packle::MsgID)MsgID::Query,
            std::bind(&DBService::query, this, std::placeholders::_1));
    }

    void query(Packle::sPtr packle) {
        LOG_INFO << "DBService::query";
        Packle::sPtr response = m_context->returnPackle();
        test::Response resp;
        resp.set_name("nagisa");
        resp.set_age(18);
        resp.add_email("xxx@xxx.com");
        response->serialize(resp);
        response->setSessionID(packle->sessionID());
    }
private:
    ServiceContext::sPtr m_context;
};

void worker() {
    auto G = Singleton<Global>::instance();
    while(true) {
        if (!G->empty()) {
            PackleQueue::sPtr que;
            PackleQueue::sPtr queue = G->pop();
            queue->swap(*que);
            auto context = que->context();
            assert(context);
            ServiceContext::setCurrentContext(context);
            context->doFork();
            while (!que->empty()) {
                Packle::sPtr packle = que->pop();
                assert(packle);
                context->dispatch(packle);
            }
            ServiceContext::setCurrentContext(nullptr);
        }
    }
}

int main() {
    initOimo();
    DBService dbService(ServiceContext::createContext("DBService"));
    QueryService queryService(ServiceContext::createContext("QueryService"));
    Thread::sPtr t1 = std::make_shared<Thread>(worker, "worker1");
    Thread::sPtr t2 = std::make_shared<Thread>(worker, "worker2");
    t1->start();
    t2->start();
    t1->join();
    t2->join();
}