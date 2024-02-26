#include "service.h"

namespace Oimo {
    void Service::handleInit(Packle::sPtr packle) {
        if (inited) {
            return;
        }
        init(packle);
        inited = true;
        if (packle->source() != 0) {
            setReturnPackle(packle);
        } else {
            m_cond.notify_all();
        }
    }
}