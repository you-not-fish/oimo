#pragma once

#include "serviceContext.h"
#include "spinLock.h"

namespace Oimo {
    class ServiceContextMgr : noncopyable {
    public:
        using CoroutinneQueue = std::queue<Coroutine::sPtr>;
        using ServiceID = uint32_t;
        static ServiceID generateServiceID();
        static ServiceContext::sPtr getContext(const std::string& name);
        static ServiceContext::sPtr getContext(ServiceID id);
        static void registerContext(ServiceContext::sPtr context);
        static void removeContext(ServiceID id);
    private:
        static std::map<std::string, ServiceContext::sPtr> namedContexts;
        static std::map<ServiceID, ServiceContext::sPtr> idContexts;
        static SpinLock lock;
    };
} // namespace Oimo