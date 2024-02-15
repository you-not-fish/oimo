#pragma once

#include <cstddef>
#include "fdContext.h"
#include "ctrlMsg.h"

namespace Oimo {
namespace Net {
    class PipeContext : public FdContext {
    public:
        PipeContext(int fd, int sendFd);
        ~PipeContext();
        void handleEvent() override;
        ssize_t writePipe(const char* data, size_t len);
    private:
        bool hasCmd();
        ssize_t readPipe(char* buf, size_t len);
        int handleStart(struct StartCtrl* ctrl);
        int m_sendFd;
    };
} // Net
}  // OImo