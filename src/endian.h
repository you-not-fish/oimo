#pragma once

#include <cstdint>

namespace Oimo {
namespace Net {
    class Endian {
    public:
        static uint16_t hostToNet16(uint16_t host);
        static uint32_t hostToNet32(uint32_t host);
        static uint64_t hostToNet64(uint64_t host);
        static uint16_t netToHost16(uint16_t net);
        static uint32_t netToHost32(uint32_t net);
        static uint64_t netToHost64(uint64_t net);
    };
}   // Net
}   // Oimo