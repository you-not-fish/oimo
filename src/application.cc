#include <cassert>
#include <sys/syscall.h>
#include "application.h"
#include "config.h"
#include "queue.h"
#include "singleton.h"
#include "socketServer.h"
#include "timerThread.h"
#include "coroutine.h"
#include "logThread.h"
#include "socketThread.h"
#include "workThread.h"
#include "serviceContextMgr.h"

namespace Oimo {
    Application::~Application() {
        stop();
    }
    void Application::init() {
        assert(Thread::currentThreadID() == 0);
        GConfig::instance();
        GQueue::instance();
        Thread::setCurrentThreadID(syscall(SYS_gettid));
        Thread::setCurrentThreadName("main");
        Coroutine::t_mainCoroutine.reset(new Coroutine());
        Coroutine::setCurrentCoroutine(Coroutine::t_mainCoroutine);
        Net::GSocketServer::instance();
    }

    int Application::run() {
        Singleton<LogThread>::instance().start();
        Singleton<WorkThread>::instance().start();
        Singleton<Net::SocketThread>::instance().start();
        Singleton<TimeThread>::instance().start();
        Singleton<TimeThread>::instance().join();
        Singleton<Net::SocketThread>::instance().join();
        Singleton<WorkThread>::instance().join();
        Singleton<LogThread>::instance().join();
        return 0;
    }

    void Application::stop() {
        Singleton<TimeThread>::instance().stop();
        Singleton<Net::SocketThread>::instance().stop();
        Singleton<WorkThread>::instance().stop();
        Singleton<LogThread>::instance().stop();
    }

    Service::sPtr Application::getService(ServiceID id) const {
        SpinLockGuard guard(lock);
        auto it = services.find(id);
        if (it != services.end()) {
            return it->second;
        }
        return nullptr;
    }

    Application::ServiceMap Application::serviceMap() const {
        SpinLockGuard guard(lock);
        return services;
    }

    void Application::registerService(ServiceID id, Service::sPtr service) {
        SpinLockGuard guard(lock);
        auto it = services.find(id);
        if (it != services.end()) {
            LOG_ERROR << "Service: " << id << " already exists";
            return;
        }
        services[id] = service;
    }

    void Application::removeService(ServiceID id) {
        SpinLockGuard guard(lock);
        auto it = services.find(id);
        if (it != services.end()) {
            ServiceContextMgr::removeContext(it->second->id());
            services.erase(it);
        }
    }
}