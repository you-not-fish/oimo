#include <iostream>
#include <thread>
#include "../src/log.h"
#include "../src/logThread.h"
#include "../src/config.h"
#include "../src/singleton.h"

using namespace Oimo;

void test() {
    LOG_INFO << 1;
    LOG_INFO << 2.5;
    LOG_INFO << 'a';
    LOG_INFO << "Hello, world!";

}

void threadFunc(int i) {
    LOG_INFO << "thread: " << i;
}

int main() {
    LogThread logThread;
    logThread.start();
    LOG_INFO << "Hello, world!";
    LOG_DEBUG << "Hello, world!";
    LOG_ERROR << "Hello, world!";
    Logger::setLogLevel(LogLevel::DEBUG);
    test();
    std::vector<std::thread> threads;
    for (int i = 0; i < 2; ++i) {
        threads.push_back(std::thread(threadFunc, i));
    }
    LOG_INFO << Singleton<Config>::instance().map().size();
    for (auto& t : threads) {
        t.join();
    }

    for (auto& it: Singleton<Config>::instance().map()) {
        LOG_INFO << it.first << " " << it.second;
    }
    return 0;
}