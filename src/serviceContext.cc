#include <cassert>
#include "serviceContextMgr.h"
#include "log.h"
#include "sysMsg.h"
#include "timeWheel.h"
namespace Oimo {
    thread_local ServiceContext::sPtr ServiceContext::t_currentContext = nullptr;
    thread_local ServiceContext::CoroutineQueue ServiceContext::t_freeQueue;

    ServiceContext::sPtr ServiceContext::createContext(const std::string& name) {
        ServiceContext::sPtr context = ServiceContextMgr::getContext(name);
        if (context) {
            LOG_ERROR << "Context for name: " << name << " already exists";
            return context;
        }
        context = std::make_shared<ServiceContext>();
        context->setName(name);
        context->setServiceID(ServiceContextMgr::generateServiceID());
        context->setMessageQueue(std::make_shared<PackleQueue>(context));
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

    void ServiceContext::doFork() {
        size_t count = m_forkingQueue.size();
        for (size_t i = 0; i < count; ++i) {
            Coroutine::sPtr coroutine = m_forkingQueue.front();
            m_forkingQueue.pop();
            coroutine->resume();
            if (coroutine->state() == Coroutine::CoroutineState::STOPPED) {
                returnCoroutine(coroutine);
            }
        }
    }

    void ServiceContext::suspend(Coroutine::sPtr coroutine) {
        assert(coroutine);
        // assert(coroutine->state() == Coroutine::CoroutineState::RUNNING);
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
        if (packle->type() == (Packle::MsgID)SystemMsgID::TIMER) {
            auto id = packle->sessionID();
            if (id == 0) {
                LOG_ERROR << "Invalid timer packle";
                return;
            }
            auto it = m_timers.find(id);
            if (it != m_timers.end()) {
                auto ctx = it->second;
                auto cor = ctx->cor;
                if (cor->state() == Coroutine::CoroutineState::RUNNING) {
                    ctx->pending++;
                } else {
                    cor->resume();
                    if (!ctx->loop) {
                        m_timers.erase(it);
                    }
                    if (cor->state() == Coroutine::CoroutineState::STOPPED) {
                        returnCoroutine(cor);
                    }
                }
            }
            return;
        }
        if (packle->isRet()) {
            if (packle->sessionID() != 0) {
                Coroutine::sPtr coroutine = getSuspendCoroutine(packle->sessionID());
                if (coroutine) {
                    m_responsePackle = packle;
                    coroutine->resume();
                    m_responsePackle.reset();
                    if (coroutine->state() == Coroutine::CoroutineState::STOPPED) {
                        returnCoroutine(coroutine);
                    }
                }
            }
        } else {
            auto it = m_handlers.find(packle->type());
            if (it != m_handlers.end()) {
                Coroutine::sPtr coroutine = getCoroutine(
                    std::bind(it->second, packle)
                );
                coroutine->resume();
                if (coroutine->state() == Coroutine::CoroutineState::STOPPED) {
                    returnCoroutine(coroutine);
                }
            } else {
                LOG_ERROR << "No handler for message type: " << packle->type();
            }
        }
        ret(packle->source());
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
        auto sessionID = self->getSession();
        packle->setSource(currentContext()->serviceID());
        packle->setSessionID(sessionID);
        dest->messageQueue()->push(packle);
        coroutine->setSid(sessionID);
        self->suspend(coroutine);
        Coroutine::yieldToSuspend();
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
        auto serviceID = currentContext() ? currentContext()->serviceID() : 0;
        packle->setSource(serviceID);
        packle->setSessionID(0);
        dest->messageQueue()->push(packle);
    }

    void ServiceContext::ret(ServiceID dest) {
        if (!m_returnPackle) return;
        ServiceContext::sPtr context = ServiceContextMgr::getContext(dest);
        if (context) {
            m_returnPackle->setIsRet(true);
            context->messageQueue()->push(m_returnPackle);
        } else {
            LOG_ERROR << "No context for serviceID: " << dest;
        }
        m_returnPackle.reset();
    }

    Coroutine::SessionID ServiceContext::getSession() {
        Coroutine::SessionID sid;
        do {
            sid = Coroutine::generateSid();
        } while (hasSession(sid));
        return sid;
    }

    Coroutine::sPtr ServiceContext::getSuspendCoroutine(Coroutine::SessionID sessionID) {
        auto it = m_suspendingPool.find(sessionID);
        if (it != m_suspendingPool.end()) {
            Coroutine::sPtr coroutine = it->second;
            assert(coroutine->state() == Coroutine::CoroutineState::SUSPENDED);
            m_suspendingPool.erase(it);
            return coroutine;
        }
        return nullptr;
    }

    void ServiceContext::addFork(Coroutine::sPtr coroutine) {
        assert(coroutine->state() == Coroutine::CoroutineState::SUSPENDED);
        m_forkingQueue.push(coroutine);
    }

    void ServiceContext::timerCallback(TimerContext::sPtr timer) {
        assert(timer->cor == Coroutine::currentCoroutine());
        while (true) {
            timer->callback();
            if (!timer->loop) break;
            if (timer->pending > 0) {
                timer->pending--;
                continue;
            }
            Coroutine::yieldToSuspend();
        }
    }

    void ServiceContext::addTimer(uint32_t delay, uint32_t interval,
        Coroutine::CoroutineFunc func) {
        Timer::sPtr timer = std::make_shared<Timer>();
        timer->delay = delay;
        timer->interval = interval;
        static uint64_t id = 0;
        timer->serv = serviceID();
        timer->session = ++id;
        TimerContext::sPtr ctx = std::make_shared<TimerContext>();
        ctx->callback = func;
        ctx->id = id;
        ctx->loop = interval > 0;
        ctx->pending = 0;
        m_timers[id] = ctx;
        if (func) {
            ctx->cor = getCoroutine(
                std::bind(&ServiceContext::timerCallback, this, ctx)
            );
        } else {
            ctx->cor = Coroutine::currentCoroutine();
        }
        GTimerWheel::instance().addTimer(timer);
        if (!func) {
            Coroutine::yieldToSuspend();
        }
    }
}