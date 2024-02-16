#include <iostream>
#include <src/application.h>
#include <src/singleton.h>
#include <src/logThread.h>
#include <src/coroutine.h>

using namespace Oimo;

void corFunc() {
    LOG_INFO << "before suspend, coroutine" << Coroutine::currentCoroutineID();
    Coroutine::yieldToSuspend();
    LOG_INFO << "after suspend, coroutine" << Coroutine::currentCoroutineID();
}

void testCoroutine() {
    LOG_INFO << "thread: " << Thread::currentThreadID();
    Coroutine::sPtr cor = std::make_shared<Coroutine>(corFunc);
    std::cout << "use count: " << cor.use_count() <<std::endl;
    cor->resume();
    LOG_INFO << "after coroutine start, thread: " << Thread::currentThreadID();
    cor->resume();
    LOG_INFO << "after coroutine resume, thread: " << Thread::currentThreadID();
}

void threadFunc() {
    LOG_INFO << "thread: " << Thread::currentThreadID();
    for (int i = 0; i < 3; ++i) {
        testCoroutine();
    }
}

int main() {
    Application app;
    app.init();
    Singleton<LogThread>::instance().start();
    threadFunc();
    std::vector<Thread::sPtr> threads;
    for (int i = 0; i < 3; ++i) {
        threads.push_back(std::make_shared<Thread>(threadFunc, "thread" + std::to_string(i)));
    }
    for (auto& t : threads) {
        t->start();
    }
    for (auto& t : threads) {
        t->join();
    }
    return 0;
}