#include <cassert>
#include "serviceContextMgr.h"

namespace Oimo {

    std::map<std::string, ServiceContext::sPtr> ServiceContextMgr::namedContexts;
    std::map<ServiceContextMgr::ServiceID, ServiceContext::sPtr> ServiceContextMgr::idContexts;
    SpinLock ServiceContextMgr::lock;
    
    ServiceContextMgr::ServiceID ServiceContextMgr::generateServiceID() {
        static ServiceID s_serviceID = 0;
        return ++s_serviceID;
    }

    ServiceContext::sPtr ServiceContextMgr::getContext(const std::string& name) {
        SpinLockGuard guard(lock);
        auto it = namedContexts.find(name);
        if (it != namedContexts.end()) {
            return it->second;
        }
        return nullptr;
    }

    ServiceContext::sPtr ServiceContextMgr::getContext(ServiceID id) {
        SpinLockGuard guard(lock);
        auto it = idContexts.find(id);
        if (it != idContexts.end()) {
            return it->second;
        }
        return nullptr;
    }

    void ServiceContextMgr::registerContext(ServiceContext::sPtr context) {
        assert(context);
        SpinLockGuard guard(lock);
        namedContexts[context->name()] = context;
        idContexts[context->serviceID()] = context;
    }
}