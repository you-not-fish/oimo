#include <iostream>
#include <vector>
#include "../src/application.h"
#include "../src/singleton.h"
#include "../src/logThread.h"

using namespace Oimo;

int main() {
    Application app;
    app.init();
    Singleton<LogThread>::instance().start();
    std::vector<Thread::sPtr> threads;
    for (int i = 0; i < 2; ++i) {
        threads.push_back(std::make_shared<Thread>(
            [i] {
                std::cout << "thread: " << i << std::endl;
                LOG_INFO << "thread: " << i;
            }, "thread" + std::to_string(i)));
    }
    std::cout << "main thread" << std::endl;
    for (auto& t : threads) {
        t->start();
    }
    for (auto& t : threads) {
        t->join();
    }
    return 0;
}