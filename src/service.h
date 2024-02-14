#pragma once

#include <memory>
#include <iostream>
#include "serviceContext.h"

namespace Oimo {
    class Service {
    public:
        using sPtr = std::shared_ptr<Service>;
        Service(ServiceContext::sPtr context = nullptr) : m_context(context) {
        }
        virtual ~Service() = default;
        virtual void init(Packle::sPtr packle) = 0;
        void registerFunc(Packle::MsgID messageID, ServiceContext::HandlerFunc handler) {
            m_context->registerHandler(messageID, handler);
        }

        template<typename T>
        void call(T dest, Packle::sPtr packle) {
            m_context->call(dest, packle);
        }
        template<typename T>
        void send(T dest, Packle::sPtr packle) {
            m_context->send(dest, packle);
        }
        ServiceContext::ServiceID id() const {
            return m_context ? m_context->serviceID() : 0;
        }
        void setContext(const ServiceContext::sPtr& context) {
            m_context = context;
        }
        ServiceContext::sPtr context() const {
            return m_context;
        }
        Packle::sPtr responsePackle() {
            return m_context->responsePackle();
        }
        void setReturnPackle(Packle::sPtr packle) {
            m_context->setReturnPackle(packle);
        }
    private:
        ServiceContext::sPtr m_context;
    };
}