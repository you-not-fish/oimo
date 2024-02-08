#include <cassert>
#include "serviceContext.h"
#include "serviceContextMgr.h"
#include "log.h"
namespace Oimo {
    thread_local ServiceContext::sPtr ServiceContext::t_currentContext = nullptr;
    thread_local ServiceContext::CoroutineQueue ServiceContext::t_freeQueue;

    ServiceContext::sPtr ServiceContext::createContext(const std::string& name) {
        ServiceContext::sPtr context = ServiceContextMgr::getContext(name);
        if (context) {
            LOG_WARN << "Context for name: " << name << " already exists";
            return context;
        }
        context = std::make_shared<ServiceContext>();
        context->setName(name);
        context->setServiceID(ServiceContextMgr::generateServiceID());
        ServiceContextMgr::registerContext(context);
        return context;
    }

    ServiceContext::~ServiceContext() {
        for (auto& it : m_suspendingPool) {
            t_freeQueue.push(it.second);
        }
    }
    void ServiceContext::fork(Coroutine::CoroutineFunc func) {
        Coroutine::sPtr coroutine = getCoroutine(func);
        m_forkingQueue.push(coroutine);
    }
    void ServiceContext::suspend(Coroutine::sPtr coroutine) {
        assert(coroutine);
        assert(coroutine->state() == Coroutine::CoroutineState::RUNNING);
        assert(m_suspendingPool.find(coroutine->sid()) == m_suspendingPool.end());
        m_suspendingPool[coroutine->sid()] = coroutine;
    }
    void ServiceContext::registerHandler(Packle::MsgID messageID, HandlerFunc handler) {
        auto it = m_handlers.find(messageID);
        if (it != m_handlers.end()) {
            LOG_WARN << "Handler for messageID: " << messageID << " already exists";
            it->second = handler;
        } else {
            m_handlers[messageID] = handler;
        }
    }
    void ServiceContext::dispatch(Packle::sPtr packle) {
        if (packle->isRet()) {
            if (packle->sessionID() != 0) {
                Coroutine::sPtr coroutine = getSuspendCoroutine(packle->sessionID());
                if (coroutine) {
                    m_responsePackle = packle;
                    coroutine->resume();
                    m_responsePackle.reset();
                }
            }
        }
    }
    
    Coroutine::sPtr ServiceContext::getCoroutine(const Coroutine::CoroutineFunc& func) {
        if (t_freeQueue.empty()) {
            return std::make_shared<Coroutine>(func);
        } else {
            Coroutine::sPtr coroutine = t_freeQueue.front();
            t_freeQueue.pop();
            coroutine->reset(func);
            return coroutine;
        }
    }
    void ServiceContext::returnCoroutine(Coroutine::sPtr coroutine) {
        assert(coroutine);
        assert(coroutine->state() != Coroutine::CoroutineState::RUNNING);
        t_freeQueue.push(coroutine);
    }

    void ServiceContext::call(ServiceID dest, Packle::sPtr packle) {
        ServiceContext::sPtr context = ServiceContextMgr::getContext(dest);
        if (context) {
            call(context, packle);
        } else {
            LOG_ERROR << "No context for serviceID: " << dest;
        }
    }
    void ServiceContext::call(std::string dest, Packle::sPtr packle) {
        ServiceContext::sPtr context = ServiceContextMgr::getContext(dest);
        if (context) {
            call(context, packle);
        } else {
            LOG_ERROR << "No context for name: " << dest;
        }
    }

    void ServiceContext::call(ServiceContext::sPtr dest, Packle::sPtr packle) {
        assert(dest);
        assert(packle);
        assert(currentContext());
        assert(!Coroutine::isMainCoroutine());
        auto coroutine = Coroutine::currentCoroutine();
        auto self = currentContext();
        auto sessionID = Coroutine::generateSid(self->serviceID());
        packle->setSource(currentContext()->serviceID());
        packle->setSessionID(sessionID);
        dest->messageQueue()->push(packle);
        coroutine->setSid(sessionID);
        self->suspend(coroutine);
        Coroutine::yieldToStopped();
    }

    void ServiceContext::send(ServiceID dest, Packle::sPtr packle) {
        ServiceContext::sPtr context = ServiceContextMgr::getContext(dest);
        if (context) {
            send(context, packle);
        } else {
            LOG_ERROR << "No context for serviceID: " << dest;
        }
    }
    void ServiceContext::send(std::string dest, Packle::sPtr packle) {
        ServiceContext::sPtr context = ServiceContextMgr::getContext(dest);
        if (context) {
            send(context, packle);
        } else {
            LOG_ERROR << "No context for name: " << dest;
        }
    }

    void ServiceContext::send(ServiceContext::sPtr dest, Packle::sPtr packle) {
        assert(dest);
        assert(packle);
        assert(currentContext());
        assert(!Coroutine::isMainCoroutine());
        packle->setSource(currentContext()->serviceID());
        packle->setSessionID(0);
        dest->messageQueue()->push(packle);
    }

    void ServiceContext::ret(ServiceID dest) {
        if (!m_returnPackle) return;
        ServiceContext::sPtr context = ServiceContextMgr::getContext(dest);
        if (context) {
            m_returnPackle->setIsRet(true);
            context->messageQueue()->push(m_responsePackle);
            m_responsePackle.reset();
        } else {
            LOG_ERROR << "No context for serviceID: " << dest;
        }
    }

    Coroutine::sPtr ServiceContext::getSuspendCoroutine(Coroutine::SessionID sessionID) {
        auto it = m_suspendingPool.find(sessionID);
        if (it != m_suspendingPool.end()) {
            Coroutine::sPtr coroutine = it->second;
            m_suspendingPool.erase(it);
            return coroutine;
        }
        return nullptr;
    }
}