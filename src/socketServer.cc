#include <unistd.h>
#include "socketServer.h"
#include "log.h"

namespace Oimo {
namespace Net {

    SocketServer::SocketServer() {
        int fds[2];
        if (pipe(fds) == -1) {
            LOG_FATAL << "create pipe failed";
            return;
        }
        m_pipeCtx.setFds(fds[0], fds[1]);
        m_loop.updateEvent(&m_pipeCtx);
    }
}
}