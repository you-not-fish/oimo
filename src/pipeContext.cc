#include <cassert>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include "pipeContext.h"
#include "log.h"
#include "sysMsg.h"
#include "socketServer.h"
#include "singleton.h"

namespace Oimo {
namespace Net {
    PipeContext::PipeContext(int fd, int sendFd)
        : FdContext(fd)
        , m_sendFd(sendFd) {}
    PipeContext::~PipeContext() {
        ::close(m_fd);
        ::close(m_sendFd);
    }

    void PipeContext::handleEvent() {
        assert(m_revents & EPOLLIN);
        while (hasCmd()) {
            char buf[256];
            uint8_t head[2];
            readPipe(reinterpret_cast<char*>(head), 2);
            char type = head[0];
            uint8_t len = head[1];
            readPipe(buf, len);
            switch (type) {
                case 'S':
                    handleStart(reinterpret_cast<struct StartCtrl*>(buf));
                    break;
                default:
                    LOG_ERROR << "PipeContext::handleEvent: unknown type";
            }
        }
    }

    ssize_t PipeContext::writePipe(const char* data, size_t len) {
        for (;;) {
            ssize_t n = ::write(m_sendFd, data, len);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                LOG_ERROR << "write error: " << ::strerror(errno);
                return 0;
            }
            return n;
        }
    }

    ssize_t PipeContext::readPipe(char* buf, size_t len) {
        for (;;) {
            ssize_t n = ::read(m_fd, buf, len);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                LOG_ERROR << "read error: " << ::strerror(errno);
                return 0;
            }
            assert(n == (ssize_t)len);
            return n;
        }
    }

    bool PipeContext::hasCmd() {
        int n = 0;
        ::ioctl(m_fd, FIONREAD, &n);
        return n > 0;
    }

    int PipeContext::handleStart(struct StartCtrl* ctrl) {
        int fd = ctrl->fd;
        auto ctx = Singleton<SocketServer>::instance().getSocketContext(fd);
        assert(ctx->isValid());
        assert(ctx->sockType() == SocketType::PLISTEN);
        enableRead();
        ctx->setSockType(SocketType::LISTEN);
        Packle::sPtr packle = std::make_shared<Packle>((Packle::MsgID)SystemMsgID::LISTENREADY);
        packle->setSessionID(ctrl->session);
        packle->setIsRet(true);
        sendProto(packle, ctx->serv());
        return 0;
    }
} // Net
}  // OImo