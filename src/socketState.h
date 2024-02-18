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
        DELETE,
        HALFCLOSE_READ,
        HALFCLOSE_WRITE,
        CLOSE
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
            case SocketType::HALFCLOSE_READ:
                return "HALFCLOSE_READ";
            case SocketType::HALFCLOSE_WRITE:
                return "HALFCLOSE_WRITE";
            default:
                return "UNKNOWN";
        }
    }

}  // Net
}  // Oimo