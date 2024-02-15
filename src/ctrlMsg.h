#pragma once

#include <cstdint>

namespace Oimo {
namespace Net {
    struct StartCtrl {
        int fd;
        uint16_t session;
    };
    struct CtrlPacket {
        uint8_t head[8];
        union ctrlMsg {
            StartCtrl start;
        } msg;
        
    };

}   // Net
}   // Oimo