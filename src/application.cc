#include <cassert>
#include <sys/syscall.h>
#include "application.h"
#include "config.h"
#include "queue.h"
#include "singleton.h"
#include "socketServer.h"
#include "coroutine.h"
#include "logThread.h"
#include "socketThread.h"
#include "workThread.h"

namespace Oimo {
    Application::~Application() {
        stop();
    }
    void Application::init() {
        assert(Thread::currentThreadID() == 0);
        Singleton<Config>::instance();
        Singleton<GlobalQueue>::instance();
        Thread::setCurrentThreadID(syscall(SYS_gettid));
        Thread::setCurrentThreadName("main");
        Coroutine::t_mainCoroutine.reset(new Coroutine());
        Coroutine::setCurrentCoroutine(Coroutine::t_mainCoroutine);
        Singleton<Net::SocketServer>::instance();
    }

    int Application::run() {
        Singleton<LogThread>::instance().start();
        Singleton<WorkThread>::instance().start();
        Singleton<Net::SocketThread>::instance().start();
        Singleton<Net::SocketThread>::instance().join();
        Singleton<WorkThread>::instance().join();
        Singleton<LogThread>::instance().join();
        return 0;
    }

    void Application::stop() {
        Singleton<Net::SocketThread>::instance().stop();
        Singleton<WorkThread>::instance().stop();
        Singleton<LogThread>::instance().stop();
    }
}