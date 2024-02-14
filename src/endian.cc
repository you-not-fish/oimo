#include "endian.h"
#include <arpa/inet.h>

namespace Oimo{
namespace Net {
    uint16_t Endian::hostToNet16(uint16_t host) {
        return ::htons(host);
    }

    uint32_t Endian::hostToNet32(uint32_t host) {
        return ::htonl(host);
    }

    uint64_t Endian::hostToNet64(uint64_t host) {
        return (((uint64_t)hostToNet32(host & 0xFFFFFFFF) << 32) | hostToNet32(host >> 32));
    }

    uint16_t Endian::netToHost16(uint16_t net) {
        return ::ntohs(net);
    }

    uint32_t Endian::netToHost32(uint32_t net) {
        return ::ntohl(net);
    }

    uint64_t Endian::netToHost64(uint64_t net) {
        return (((uint64_t)netToHost32(net & 0xFFFFFFFF) << 32) | netToHost32(net >> 32));
    }

} // namespace Net
} // namespace Oimo
