#pragma once

namespace Oimo {
    enum class SystemMsgID {
        INIT = 0XFFFF,
        TIMER = 0XFF00,
        NEWCONN = 0XFF01,
        LISTENREADY = 0XFF02,
        DATA = 0XFF03,
        CLOSEREAD = 0XFF04,
        CLOSED = 0XFF05,
        ERROR = 0XFF06
    };
}