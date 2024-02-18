#pragma once

#include <cstdint>

namespace Oimo {
namespace Net {
    struct StartCtrl {
        int fd;
        uint16_t session;
    };

    struct DataCtrl {
        int fd;
        char *buf;
        size_t len;
    };

    struct CloseCtrl {
        int fd;
        uint16_t session;
    };

    struct CtrlPacket {
        uint8_t head[8];
        union ctrlMsg {
            StartCtrl start;
            DataCtrl data;
            CloseCtrl close;
        } msg;
        
    };

}   // Net
}   // Oimo