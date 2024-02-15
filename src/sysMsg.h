#pragma once

namespace Oimo {
    enum class SystemMsgID {
        INIT = 0XFFFF,
        NEWCONN = 0XFF01,
        LISTENREADY = 0XFF02
    };
}