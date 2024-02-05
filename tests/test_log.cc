#include <iostream>
#include <thread>
#include "../src/log.h"
#include "../src/logThread.h"
#include "../src/config.h"
#include "../src/singleton.h"

using namespace Oimo;

void test() {
    LOG_TRACE << std::string("this is a string");
    LOG_INFO << 1;
    LOG_WARN << 2.5;
    LOG_ERROR << 'a';
    LOG_DEBUG << "Hello, world!";
    LOG_FATAL << "Goodbye, world!";

}

void threadFunc(int i) {
    LOG_INFO << "thread: " << i;
}

int main() {
    LogThread logThread;
    logThread.start();
    Logger::setLogLevel(LogLevel::DEBUG);
    test();
    std::vector<std::thread> threads;
    for (int i = 0; i < 2; ++i) {
        threads.push_back(std::thread(threadFunc, i));
    }
    LOG_INFO << "config count : "
            << Singleton<Config>::instance().map().size();
    for (auto& t : threads) {
        t.join();
    }

    for (auto& it: Singleton<Config>::instance().map()) {
        LOG_INFO << it.first << " " << it.second;
    }
    return 0;
}