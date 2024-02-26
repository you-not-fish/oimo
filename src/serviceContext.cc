#include <cassert>
#include "serviceContextMgr.h"
#include "log.h"
#include "sysMsg.h"
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
        auto session = packle->sessionID();
        auto source = packle->source();
        if (packle->type() == (Packle::MsgID)SystemMsgID::TIMER) {
            if (session == 0) {
                LOG_ERROR << "Invalid timer packle";
                return;
            }
            auto it = m_timers.find(session);
            if (it != m_timers.end()) {
                auto ctx = it->second;
                // LOG_DEBUG << "ctx use_count: " << ctx.use_count() << ", timer use_count: " << ctx->timer.use_count() << ", dispatch timer";
                auto cor = ctx->cor;
                if (cor->state() == Coroutine::CoroutineState::RUNNING) {
                    ctx->pending++;
                } else {
                    cor->resume();
                    // LOG_DEBUG << "ctx use_count: " << ctx.use_count() << ", timer use_count: " << ctx->timer.use_count() << ", resume timer";
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
            if (session != 0) {
                Coroutine::sPtr coroutine = getSuspendCoroutine(session);
                if (coroutine) {
                    packle->setIsRet(false);
                    m_responsePackle = packle;
                    coroutine->resume();
                    source = coroutine->reservSrc();
                    m_responsePackle.reset();
                    if (coroutine->state() == Coroutine::CoroutineState::STOPPED) {
                        returnCoroutine(coroutine);
                    }
                } else {
                    auto ctx = ServiceContextMgr::getContext(packle->source());
                    LOG_ERROR << "No coroutine in service " << name() << " for sessionID: " << packle->sessionID()
                        << ", source: " << (ctx ? ctx->name() : "unknown") << "("
                        << packle->source() << ")"
                        << ", type: " << packle->type();
                }
            }
        } else {
            uint16_t type = packle->type();
            auto it = m_handlers.find(type);
            if (it != m_handlers.end()) {
                Coroutine::sPtr coroutine = getCoroutine(
                    std::bind(it->second, packle)
                );
                if (session != 0) {
                    coroutine->setReservSid(session);
                    coroutine->setReservSrc(source);
                }
                coroutine->resume();
                source = coroutine->reservSrc();
                if (coroutine->state() == Coroutine::CoroutineState::STOPPED) {
                    returnCoroutine(coroutine);
                }
            } else {
                auto ctx = ServiceContextMgr::getContext(packle->source());
                std::string handlers;
                for (auto& it : m_handlers) {
                    handlers += std::to_string(it.first) + ", ";
                }
                LOG_ERROR << "Service " << name() << " has no handler for messageID: "
                    << packle->type() << ", source: " << (ctx ? ctx->name() : "unknown")
                    << "Handlers: " << handlers;
            }
        }
        if (m_returnPackle){
            ret(source);
        }
    }
    
    Coroutine::sPtr ServiceContext::getCoroutine(const Coroutine::CoroutineFunc& func) {
        if (t_freeQueue.empty()) {
            return std::make_shared<Coroutine>(func);
        } else {
            Coroutine::sPtr coroutine = t_freeQueue.front();
            t_freeQueue.pop();
            assert(coroutine->state() != Coroutine::CoroutineState::RUNNING);
            coroutine->reset(func);
            return coroutine;
        }
    }
    void ServiceContext::returnCoroutine(Coroutine::sPtr coroutine) {
        assert(coroutine);
        t_freeQueue.push(coroutine);
    }

    void ServiceContext::setReturnPackle(Packle::sPtr packle) {
        auto cor = Coroutine::currentCoroutine();
        packle->setSessionID(cor->reservSid());
        m_returnPackle = packle;
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
        LOG_TRACE << self->name() << " call " << dest->name() << ", sessionID: " << sessionID;
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

    void ServiceContext::ret(ServiceContext::ServiceID dest) {
        ServiceContext::sPtr context = ServiceContextMgr::getContext(dest);
        if (context) {
            m_returnPackle->setType((Packle::MsgID)SystemMsgID::RET);
            m_returnPackle->setIsRet(true);
            m_returnPackle->setSource(serviceID());
            LOG_TRACE << name() << " ret " << context->name()
                << ", sessionID: " << m_returnPackle->sessionID();
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

    void ServiceContext::timerCallback(TimerContext::wPtr ctx) {
        // LOG_DEBUG << "ctx use_count: " << ctx.use_count() << ", timer callback";
        while (true) {
            auto timer = ctx.lock();
            if (!timer) return;
            assert(timer->cor == Coroutine::currentCoroutine());
            // LOG_DEBUG << "ctx use_count: " << timer.use_count() << ", timer use_count: " << timer->timer.use_count() << ", callback";
            timer->callback();
            // LOG_DEBUG << "ctx use_count: " << timer.use_count() << ", timer use_count: " << timer->timer.use_count() << ", callback";
            if (!timer->loop) break;
            if (timer->pending > 0) {
                timer->pending--;
                continue;
            }
            timer.reset();
            Coroutine::yieldToSuspend();
        }
    }

    uint64_t ServiceContext::addTimer(uint32_t delay, uint32_t interval,
        Coroutine::CoroutineFunc func) {
        Timer::sPtr timer = std::make_shared<Timer>();
        timer->delay = delay;
        timer->interval = interval;
        static uint64_t id = 0;
        timer->serv = serviceID();
        timer->session = ++id;
        TimerContext::sPtr ctx = std::make_shared<TimerContext>();
        ctx->timer = timer;
        ctx->callback = func;
        ctx->id = id;
        ctx->loop = interval > 0;
        ctx->pending = 0;
        m_timers[id] = ctx;
        if (func) {
            // LOG_DEBUG << "ctx use_count: " << ctx.use_count() << ", timer use_count: " << ctx->timer.use_count() << ", create timer";
            TimerContext::wPtr wctx = ctx;
            ctx->cor = getCoroutine(
                std::bind(&ServiceContext::timerCallback, this, wctx)
            );
        } else {
            ctx->cor = Coroutine::currentCoroutine();
        }
        GTimerWheel::instance().addTimer(timer);
        if (!func) {
            Coroutine::yieldToSuspend();
        }
        // LOG_DEBUG << "ctx use_count: " << ctx.use_count() << ", timer use_count: " << ctx->timer.use_count() << ", add timer";
        return id;
    }

    void ServiceContext::removeTimer(uint64_t id) {
        auto it = m_timers.find(id);
        if (it != m_timers.end()) {
            auto ctx = it->second;
            // ctx->cor->setCallback(nullptr);
            // ctx->timer = nullptr;
            returnCoroutine(ctx->cor);
            m_timers.erase(it);
            // LOG_DEBUG << "ctx use_count: " << ctx.use_count() << ", timer use_count: " << ctx->timer.use_count() << ", remove timer";
        }
    }
}