#pragma once


namespace Oimo {
namespace Net {
    enum class EventType {
        NEW,
        ADDED
    };
    enum class SocketType {
        NEW,
        PLISTEN,
        LISTEN,
        PACCEPT,
        ACCEPT,
        DELETE
    };

}  // Net
}  // Oimo