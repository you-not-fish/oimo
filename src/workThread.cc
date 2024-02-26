#include <cassert>
#include "workThread.h"
#include "serviceContext.h"
#include "config.h"
#include "queue.h"
#include "singleton.h"

namespace Oimo {

    bool WorkThread::running = false;
    WorkThread::WorkThread() {
        int n = Singleton<Config>::instance().getInt("thread.workthread_count", 6);
        for (int i = 0; i < n; ++i) {
            m_threads.push_back(
                std::make_shared<Thread>(
                    work, std::string("workthread_") + std::to_string(i)));
        }
    }

    WorkThread::~WorkThread() {
        if (running) {
            stop();
            join();
        }
    }

    void WorkThread::start() {
        running = true;
        for (auto& thread : m_threads) {
            thread->start();
        }
    }

    void WorkThread::join() {
        for (auto& thread : m_threads) {
            thread->join();
        }
    }

    void WorkThread::stop() {
        running = false;
    }

    void WorkThread::work() {
        auto& G = Singleton<GlobalQueue>::instance();
        while(running) {
            if (!G.empty()) {
                auto que = G.pop();
                if (!que) {
                    continue;
                }
                que->startProcess();
                auto ctx = que->context();
                auto context = ctx.lock();
                if (!context) {
                    continue;
                }
                ServiceContext::setCurrentContext(context);
                context->doFork();
                int size = (int)que->size();
                for (int i = 0; i < size; ++i) {
                    Packle::sPtr packle = que->pop();
                    assert(packle);
                    context->dispatch(packle);
                }
                que->finishProcess(context->hasFork());
                ServiceContext::setCurrentContext(nullptr);
            }
        }
    }
}