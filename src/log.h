#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <string.h>

namespace Oimo {
    enum class LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    inline std::string logLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }

    class LogBuffer {
    public:
        using sPtr = std::shared_ptr<LogBuffer>;
        void append(const char* data, int len) {
            if (m_offset + len > sizeof(m_buffer)) {
                return;
            }
            std::copy(data, data + len, m_buffer + m_offset);
            m_offset += len;
        }
    private:
        char m_buffer[1024];
        int m_offset{0};
    };

    class LogStream {
    public:
        LogStream() : m_pBuffer(std::make_shared<LogBuffer>()) {}
        LogStream& operator<<(const std::string& value) {
            m_pBuffer->append(value.c_str(), value.size());
            return *this;
        }
        LogStream& operator<<(const char* value) {
            m_pBuffer->append(value, strlen(value));
            return *this;
        }

        LogStream& operator<<(char value) {
            m_pBuffer->append(&value, 1);
            return *this;
        }

        #define DEFINE_STREAM_OPERATOR(type) \
            LogStream& operator<<(type value) { \
                *this << std::to_string(value); \
                return *this; \
            }

        DEFINE_STREAM_OPERATOR(int)
        DEFINE_STREAM_OPERATOR(unsigned int)
        DEFINE_STREAM_OPERATOR(long)
        DEFINE_STREAM_OPERATOR(unsigned long)
        DEFINE_STREAM_OPERATOR(long long)
        DEFINE_STREAM_OPERATOR(unsigned long long)
        DEFINE_STREAM_OPERATOR(float)
        DEFINE_STREAM_OPERATOR(double)
        DEFINE_STREAM_OPERATOR(long double)

        #undef DEFINE_STREAM_OPERATOR

        LogStream& operator<<(bool value) {
            *this << (value ? "true" : "false");
            return *this;
        }

        LogStream& operator<<(LogLevel level) {
            *this << logLevelToString(level);
            return *this;
        }

        LogStream& operator<<(short value) {
            *this << static_cast<int>(value);
            return *this;
        }

        LogStream& operator<<(unsigned short value) {
            *this << static_cast<unsigned int>(value);
            return *this;
        }

        LogBuffer::sPtr buffer() { return m_pBuffer; }

    private:
        LogBuffer::sPtr m_pBuffer;
    };

    class Logger {
    public:
        Logger(LogLevel level, const char* file, int line, const char* func);
        ~Logger();
        static LogLevel logLevel() { return g_logLevel; }
        static void setLogLevel(LogLevel level) {
            g_logLevel = level;
        }

        LogStream& stream() { return m_stream; }
    private:
        LogStream m_stream;
    };

    
    extern LogLevel g_logLevel;
    extern std::mutex g_logMutex;
    extern std::vector<LogBuffer::sPtr> g_logBuffers;
}