#pragma once

#include <memory>

namespace Oimo {
    class Packle {
    public:
        using sPtr = std::shared_ptr<Packle>;
        using MsgID = uint16_t;
        Packle(MsgID type = 0, uint32_t source = 0,
            char* buf = nullptr, size_t size = 0)
            : m_type(type)
            , m_source(source)
            , m_sessionID(0)
            , m_buf(buf)
            , m_size(size)
            , m_isRet(false) {
        }
        virtual ~Packle() {
            if (m_buf) {
                delete[] m_buf;
            }
        }

        template <typename T>
        void serialize(const T& v) {
            size_t size = v.ByteSizeLong();
            if (!m_buf || m_size < size) {
                if (m_buf) {
                    delete[] m_buf;
                }
                m_buf = new char[size];
                m_size = size;
            }
            v.SerializeToArray(m_buf, size);
        }

        template <typename T>
        T deserialize() {
            T v;
            v.ParseFromArray(m_buf, m_size);
            return v;
        }

        void reset(uint16_t type, uint32_t source, char* buf, size_t size) {
            m_type = type;
            m_source = source;
            m_sessionID = 0;
            m_buf = buf;
            m_size = size;
            m_isRet = false;
        }

        uint16_t type() const {
            return m_type;
        }

        uint32_t source() const {
            return m_source;
        }

        char* buf() const {
            return m_buf;
        }

        size_t size() const {
            return m_size;
        }

        void setType(uint16_t type) {
            m_type = type;
        }

        void setSource(uint32_t source) {
            m_source = source;
        }

        void setBuf(char* buf) {
            m_buf = buf;
        }

        void setSize(size_t size) {
            m_size = size;
        }

        Coroutine::SessionID sessionID() const {
            return m_sessionID;
        }

        void setSessionID(Coroutine::SessionID sessionID) {
            m_sessionID = sessionID;
        }

        bool isRet() const {
            return m_isRet;
        }

        void setIsRet(bool isRet) {
            m_isRet = isRet;
        }

    private:
        uint16_t m_type;
        uint32_t m_source;
        Coroutine::SessionID m_sessionID;
        char* m_buf;
        size_t m_size;
        bool m_isRet;
    };
} // namespace Oim