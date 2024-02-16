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

    inline const char* SocketType2String(SocketType type) {
        switch (type) {
            case SocketType::NEW:
                return "NEW";
            case SocketType::PLISTEN:
                return "PLISTEN";
            case SocketType::LISTEN:
                return "LISTEN";
            case SocketType::PACCEPT:
                return "PACCEPT";
            case SocketType::ACCEPT:
                return "ACCEPT";
            case SocketType::DELETE:
                return "DELETE";
            default:
                return "UNKNOWN";
        }
    }

}  // Net
}  // Oimo