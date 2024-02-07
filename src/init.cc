#include <cassert>
#include <sys/syscall.h>
#include <unistd.h>
#include "config.h"
#include "thread.h"
#include "coroutine.h"
#include "logThread.h"
#include "queue.h"
#include "singleton.h"

namespace Oimo {
    void initOimo() {
        assert(Thread::currentThreadID() == 0);
        Singleton<Config>::instance();
        Singleton<GlobalQueue>::instance();
        Thread::setCurrentThreadID(syscall(SYS_gettid));
        Thread::setCurrentThreadName("main");
        Coroutine::t_mainCoroutine.reset(new Coroutine());
        Coroutine::setCurrentCoroutine(Coroutine::t_mainCoroutine);
        Singleton<LogThread>::instance().start();
    }
}