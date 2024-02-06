#include "config.h"
#include "thread.h"
#include "logThread.h"
#include <unistd.h>
#include <cassert>
#include <sys/syscall.h>
#include "singleton.h"

namespace Oimo {
    void initOimo() {
        assert(Thread::currentThreadID() == 0);
        Singleton<Config>::instance();
        Thread::setCurrentThreadID(syscall(SYS_gettid));
        Thread::setCurrentThreadName("main");
        Singleton<LogThread>::instance().start();
    }
}