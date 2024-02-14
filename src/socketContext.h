#pragma once

#include "socket.h"

namespace Oimo {
namespace Net {
    class SocketContext {
    public:
        SocketContext();
        ~SocketContext();
    private:
        Socket m_sock;
    };
}   // Net
}   // Oimo