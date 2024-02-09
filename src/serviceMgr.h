#pragma once

#include <map>
#include "service.h"

namespace Oimo {
    class ServiceMgr {
    public:
        using ServiceID = uint32_t;
        using ServiceMap = std::map<ServiceID, Service::sPtr>;
        static void registerService(ServiceID id, Service::sPtr service);
        static Service::sPtr getService(ServiceID id);
    private:
        static ServiceMap services;
        static SpinLock lock;
    };

    template <typename T>
    T newService(const std::string& name) {
        static_assert(std::is_base_of<Service, T>::value, "T must be derived from Service");
        ServiceContext::sPtr context = ServiceContext::createContext(name);
        std::shared_ptr<T> service = std::make_shared<T>();
        service->setContext(context);
        ServiceMgr::registerService(context->serviceID(), service);
        service->registerFunc((Packle::MsgID)SystemMsgID::INIT, std::bind(&T::init, service, std::placeholders::_1));
        Packle::sPtr packle = std::make_shared<Packle>((Packle::MsgID)SystemMsgID::INIT);
        ServiceContext::send(context, packle);
        return *service;
    }
} // namespace Oimo