#include <iostream>
#include <vector>
#include "../src/log.h"
#include "../src/thread.h"
#include "../src/init.h"

using namespace Oimo;

int main() {
    initOimo();
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