#pragma once

#include <cstddef>

namespace Oimo {
    class RingBuffer {
    public:
        RingBuffer(size_t size = 1024);
        ~RingBuffer();
        size_t write(const char* data, size_t len);
        size_t read(char* data, size_t len);
        size_t size() const;
        size_t remain() const;
        size_t used() const;
        void clear();
    private:
        char* m_buffer;
        size_t m_size;
        size_t m_head;
        size_t m_tail;
    };
} // Oimo