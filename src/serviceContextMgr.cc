#include <cassert>
#include "serviceContextMgr.h"
#include "packle.h"

namespace Oimo {
    ServiceContextMgr::ServiceID ServiceContextMgr::generateServiceID() {
        static ServiceID s_serviceID = 0;
        return ++s_serviceID;
    }

    ServiceContext::sPtr ServiceContextMgr::getContext(const std::string& name) {
        auto it = namedContexts.find(name);
        if (it != namedContexts.end()) {
            return it->second;
        }
        return nullptr;
    }

    ServiceContext::sPtr ServiceContextMgr::getContext(ServiceID id) {
        auto it = idContexts.find(id);
        if (it != idContexts.end()) {
            return it->second;
        }
        return nullptr;
    }

    void ServiceContextMgr::registerContext(ServiceContext::sPtr context) {
        assert(context);
        namedContexts[context->name()] = context;
        idContexts[context->serviceID()] = context;
    }
}