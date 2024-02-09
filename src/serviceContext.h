#pragma once

#include <memory>
#include <string>
#include <map>
#include <queue>
#include <functional>
#include "packle.h"
#include "queue.h"
#include "coroutine.h"

namespace Oimo {
    
    enum class SystemMsgID : Packle::MsgID {
        INIT = 0XFFFF
    };
    
    class ServiceContext {
    public:
        using sPtr = std::shared_ptr<ServiceContext>;
        using ServiceID = uint32_t;
        using HandlerFunc = std::function<void(Packle::sPtr)>;
        using CoroutinePool = std::map<Coroutine::SessionID, Coroutine::sPtr>;
        using CoroutineQueue = std::queue<Coroutine::sPtr>;
        ServiceContext() = default;
        ~ServiceContext();
        static ServiceContext::sPtr createContext(const std::string& name);
        void fork(Coroutine::CoroutineFunc func);
        void doFork();
        void suspend(Coroutine::sPtr coroutine);
        void registerHandler(Packle::MsgID messageID, HandlerFunc handler);
        void dispatch(Packle::sPtr packle);
        static Coroutine::sPtr getCoroutine(const Coroutine::CoroutineFunc& func);
        static void returnCoroutine(Coroutine::sPtr coroutine);
        ServiceID serviceID() const { return m_serviceID; }
        void setServiceID(ServiceID serviceID) { m_serviceID = serviceID; }
        std::string name() const { return m_name; }
        void setName(std::string name) { m_name = name; }
        void setReturnPackle(Packle::sPtr packle) { m_returnPackle = packle;}
        Packle::sPtr returnPackle() const { return m_returnPackle; }
        void setResponsePackle(Packle::sPtr packle) { m_responsePackle = packle; }
        Packle::sPtr responsePackle() const { return m_responsePackle; }
        static void call(ServiceID dest, Packle::sPtr packle);
        static void call(std::string dest, Packle::sPtr packle);
        static void call(ServiceContext::sPtr dest, Packle::sPtr packle);
        static void send(ServiceID dest, Packle::sPtr packle);
        static void send(std::string dest, Packle::sPtr packle);
        static void send(ServiceContext::sPtr dest, Packle::sPtr packle);
        void ret(ServiceID dest);
        Coroutine::sPtr getSuspendCoroutine(Coroutine::SessionID sessionID);
        static ServiceContext::sPtr currentContext() { return t_currentContext; }
        static void setCurrentContext(ServiceContext::sPtr context) {
            t_currentContext = context;
        }
        PackleQueue::sPtr messageQueue() const { return m_messageQueue; }
        void setMessageQueue(PackleQueue::sPtr messageQueue) {
            m_messageQueue = messageQueue;
        }
    protected:
        static thread_local ServiceContext::sPtr t_currentContext;
        std::string m_name;
        ServiceID m_serviceID;
        Packle::sPtr m_responsePackle;
        Packle::sPtr m_returnPackle;
        PackleQueue::sPtr m_messageQueue;
        CoroutinePool m_suspendingPool;
        CoroutineQueue m_forkingQueue;
        static thread_local CoroutineQueue t_freeQueue;
        std::map<Packle::MsgID, HandlerFunc> m_handlers;
    };
} // namespace Oimo