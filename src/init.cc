#include "config.h"
#include "thread.h"
#include "logThread.h"
#include <unistd.h>
#include <cassert>
#include <sys/syscall.h>
#include "singleton.h"

namespace Oimo {
    void initOimo() {
        assert(t_currentThreadID == 0);
        Singleton<Config>::instance();
        t_currentThreadID = ::syscall(SYS_gettid);
        t_currentThreadName = "main";
        Singleton<LogThread>::instance().start();
    }
}