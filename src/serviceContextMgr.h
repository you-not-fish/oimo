#pragma once

#include "serviceContext.h"

namespace Oimo {
    class ServiceContextMgr : noncopyable {
    public:
        using CoroutinneQueue = std::queue<Coroutine::sPtr>;
        using ServiceID = uint32_t;
        static ServiceID generateServiceID();
        static ServiceContext::sPtr getContext(const std::string& name);
        static ServiceContext::sPtr getContext(ServiceID id);
        static void registerContext(ServiceContext::sPtr context);
    private:
        static std::map<std::string, ServiceContext::sPtr> namedContexts;
        static std::map<ServiceID, ServiceContext::sPtr> idContexts;
    };
} // namespace Oimo