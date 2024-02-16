#include <cstring>
#include "ringBuffer.h"

namespace Oimo {
    RingBuffer::RingBuffer(size_t size)
        : m_buffer(new char[size])
        , m_size(size)
        , m_head(0)
        , m_tail(0) {
    }
    RingBuffer::~RingBuffer() {
        delete[] m_buffer;
    }

    size_t RingBuffer::write(const char* data, size_t len) {
        if (len == 0) {
            return 0;
        }
        if (len > remain()) {
            size_t newSize = m_size * 2;
            while (len > newSize - used()) {
                newSize *= 2;
            }
            char* newBuffer = new char[newSize];
            if (m_head < m_tail) {
                memcpy(newBuffer, m_buffer + m_head, m_tail - m_head);
            } else {
                memcpy(newBuffer, m_buffer + m_head, m_size - m_head);
                memcpy(newBuffer + m_size - m_head, m_buffer, m_tail);
            }
            delete[] m_buffer;
            m_buffer = newBuffer;
            m_size = newSize;
            m_head = 0;
            m_tail = used();
        }
        if (m_head < m_tail) {
            size_t right = m_size - m_tail;
            if (right >= len) {
                memcpy(m_buffer + m_tail, data, len);
                m_tail += len;
            } else {
                memcpy(m_buffer + m_tail, data, right);
                memcpy(m_buffer, data + right, len - right);
                m_tail = len - right;
            }
        } else {
            memcpy(m_buffer + m_tail, data, len);
            m_tail += len;
        }
        return len;
    }

    size_t RingBuffer::read(char* data, size_t len) {
        if (len == 0) {
            return 0;
        }
        if (len > used()) {
            len = used();
        }
        if (m_head < m_tail) {
            memcpy(data, m_buffer + m_head, len);
            m_head += len;
        } else {
            size_t right = m_size - m_head;
            if (right >= len) {
                memcpy(data, m_buffer + m_head, len);
                m_head += len;
            } else {
                memcpy(data, m_buffer + m_head, right);
                memcpy(data + right, m_buffer, len - right);
                m_head = len - right;
            }
        }
        if (m_head == m_tail) {
            m_head = m_tail = 0;
        }
        return len;
    }

    size_t RingBuffer::size() const {
        return m_size;
    }

    size_t RingBuffer::remain() const {
        if (m_head < m_tail) {
            return m_size - m_tail + m_head - 1;
        } else {
            return m_head - m_tail - 1;
        }
    }

    size_t RingBuffer::used() const {
        if (m_head <= m_tail) {
            return m_tail - m_head;
        } else {
            return m_size - m_head + m_tail;
        }
    }

    void RingBuffer::clear() {
        m_head = m_tail = 0;
    }
}