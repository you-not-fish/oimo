#include "config.h"
#include "singleton.h"

namespace Oimo {
    class GlobalInit {
    public:
        GlobalInit()
        {
            Singleton<Config>::instance();
        }
    };

    static GlobalInit g_init;
}