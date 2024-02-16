#include <iostream>
#include <src/application.h>
#include <src/singleton.h>
#include <src/queue.h>
#include <src/logThread.h>
#include <src/packle.h>
#include "protobuf/test.pb.h"

using namespace Oimo;

void threadFunc() {
    while (true) {
        if (!Singleton<GlobalQueue>::instance().empty()) {
            PackleQueue::sPtr packleQueue = Singleton<GlobalQueue>::instance().pop();
            if (packleQueue) {
                while (!packleQueue->empty()) {
                    Packle::sPtr packle = packleQueue->pop();
                    if (packle) {
                        test::TestMessage testMessage
                            = packle->deserialize<test::TestMessage>();
                        LOG_INFO << "id: " << testMessage.id();
                        LOG_INFO << "name: " << testMessage.name();
                        for (int i = 0; i < testMessage.email_size(); ++i) {
                            LOG_INFO << "email: " << testMessage.email(i);
                        }
                    }
                }
            }
        }
        break;
    }
}

int main() {
    Application app;
    app.init();
    Singleton<LogThread>::instance().start();
    Thread::sPtr t = std::make_shared<Thread>(threadFunc, "test");
    t->start();
    test::TestMessage testMessage;
    testMessage.set_id(1);
    testMessage.set_name("test");
    testMessage.add_email("test@example.com");

    Packle::sPtr packle = std::make_shared<Packle>();
    packle->serialize(testMessage);
    PackleQueue::sPtr packleQueue = std::make_shared<PackleQueue>();
    packleQueue->push(packle);
    t->join();
    return 0;
}