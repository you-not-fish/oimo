#pragma once
#include "socketState.h"
#include "packle.h"
namespace Oimo {
namespace Net {
    class FdContext {
    public:
        FdContext(int fd = -1);
        virtual ~FdContext();
        virtual void handleEvent() = 0;
        int fd() const { return m_fd; }
        void setType(EventType type) { m_type = type; }
        EventType type() const { return m_type; }
        int events() const { return m_events; }
        void setRevents(int revents) { m_revents = revents; }
        bool isWriting() const { return m_events & kWriteEvent; }
        bool isReading() const { return m_events & kReadEvent; }
        void enableRead() { m_events |= kReadEvent;update();}
        void disableRead() { m_events &= ~kReadEvent;update();}
        void enableWrite() { m_events |= kWriteEvent;update();}
        void disableWrite() { m_events &= ~kWriteEvent;update();}
        void enableReadWrite() { m_events |= kReadEvent | kWriteEvent;update();}
        void disableAll() { m_events = kNoneEvent;update();}
        bool isNoneEvent() const { return m_events == kNoneEvent; }
    protected:
        void update();
        void sendProto(const Oimo::Packle::sPtr& packle, uint32_t serv);
        int m_fd {-1};
        EventType m_type {EventType::NEW};
        int m_events {0};
        int m_revents {0};

        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;
    };
}   // Net
}   // Oimo