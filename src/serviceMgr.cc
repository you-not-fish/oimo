#include "serviceMgr.h"

namespace Oimo {

    ServiceMgr::ServiceMap ServiceMgr::services;
    SpinLock ServiceMgr::lock;
    void ServiceMgr::registerService(ServiceID id, Service::sPtr service) {
        SpinLockGuard guard(lock);
        services[id] = service;
    }

    Service::sPtr ServiceMgr::getService(ServiceID id) {
        SpinLockGuard guard(lock);
        auto iter = services.find(id);
        if (iter != services.end()) {
            return iter->second;
        }
        return nullptr;
    }
} // namespace Oimo
