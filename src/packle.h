#pragma once

#include <memory>

namespace Oimo {
    class Packle {
    public:
        using sPtr = std::shared_ptr<Packle>;
        Packle(uint16_t type = 0, uint32_t source = 0,
            char* buf = nullptr, size_t size = 0)
            : m_type(type)
            , m_source(source)
            , m_buf(buf)
            , m_size(size) {
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
            m_buf = buf;
            m_size = size;
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

    private:
        uint16_t m_type;
        uint32_t m_source;
        char* m_buf;
        size_t m_size;
    };
} // namespace Oim