#pragma once

#include "fdContext.h"

namespace Oimo {
namespace Net {
    class TimerContext : public FdContext {
    public:
        TimerContext() = default;
        virtual ~TimerContext() = default;
        void handleEvent() override;
    private:
        
    };
} // namespace Net
} // namespace Oimo