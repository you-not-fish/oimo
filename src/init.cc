#include "config.h"
#include "thread.h"
#include <unistd.h>
#include <sys/syscall.h>
#include "singleton.h"

namespace Oimo {
    class GlobalInit {
    public:
        GlobalInit()
        {
            Singleton<Config>::instance();
            t_currentThreadID = ::syscall(SYS_gettid);
            t_currentThreadName = "main";
        }
    };

    static GlobalInit g_init;
}