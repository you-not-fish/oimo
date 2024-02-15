#pragma once
#include "socketState.h"
namespace Oimo {
namespace Net {
    class EventLoop;
    class FdContext {
    public:
        FdContext(EventLoop* loop = nullptr, int fd = -1);
        virtual ~FdContext();
        virtual void handleEvent() = 0;
        void setLoop(EventLoop* loop) { m_loop = loop; }
        int fd() const { return m_fd; }
        void setType(EventType type) { m_type = type; }
        EventType type() const { return m_type; }
        int events() const { return m_events; }
        void setRevents(int revents) { m_revents = revents; }
        void enableRead() { m_events |= kReadEvent;update();}
        void disableRead() { m_events &= ~kReadEvent;update();}
        void enableWrite() { m_events |= kWriteEvent;update();}
        void disableWrite() { m_events &= ~kWriteEvent;update();}
        void enableReadWrite() { m_events |= kReadEvent | kWriteEvent;update();}
        void disableAll() { m_events = kNoneEvent;update();}
        bool isNoneEvent() const { return m_events == kNoneEvent; }
    protected:
        void update();
        EventLoop* m_loop;
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