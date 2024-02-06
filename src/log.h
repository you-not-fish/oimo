#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
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
            if (m_offset + len > static_cast<int>(sizeof(m_buffer))) {
                return;
            }
            std::copy(data, data + len, m_buffer + m_offset);
            m_offset += len;
        }

        std::string toString() const {
            return std::string(m_buffer, m_offset);
        }

        char* data() { return m_buffer; }
        int length() const { return m_offset; }
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
        static LogLevel logLevel();
        static void setLogLevel(LogLevel level);

        LogStream& stream() { return m_stream; }
    private:
        void logTime();
        LogLevel m_level;
        LogStream m_stream;
    };

    
    #define LOG_WITH_LEVEL(level) \
        if (level >= Oimo::Logger::logLevel()) \
            Oimo::Logger(level, __FILE__, __LINE__, __func__).stream()

    #define LOG_TRACE LOG_WITH_LEVEL(Oimo::LogLevel::TRACE)
    #define LOG_DEBUG LOG_WITH_LEVEL(Oimo::LogLevel::DEBUG)
    #define LOG_INFO LOG_WITH_LEVEL(Oimo::LogLevel::INFO)
    #define LOG_WARN LOG_WITH_LEVEL(Oimo::LogLevel::WARN)
    #define LOG_ERROR LOG_WITH_LEVEL(Oimo::LogLevel::ERROR)
    #define LOG_FATAL LOG_WITH_LEVEL(Oimo::LogLevel::FATAL)

    class LogFile {
    public:
        using uPtr = std::unique_ptr<LogFile>;
        LogFile(const std::string& fileName);
        ~LogFile();
        void append(const char* data, int len);
        void flush();
        void rollFile();

        int rollPerSeconds() const { return m_rollPerSeconds; }
        void setRollPerSeconds(int rollPerSeconds) { m_rollPerSeconds = rollPerSeconds; }
        int flushInterval() const { return m_flushInterval; }
        void setFlushInterval(int flushInterval) { m_flushInterval = flushInterval; }
        int checkEveryN() const { return m_checkEveryN; }
        void setCheckEveryN(int checkEveryN) { m_checkEveryN = checkEveryN; }
        int rollSize() const { return m_rollSize; }
        void setRollSize(int rollSize) { m_rollSize = rollSize; }

    private:
        void writeToFile(const char* data, int len);
        std::string formatFileName(time_t now);
        std::string m_fileName;
        FILE* m_fp;
        time_t m_lastFlushTime;
        time_t m_lastRollTime;
        time_t m_startOfPeriod;
        off_t m_writtenBytes;
        int m_count{0};
        int m_rollPerSeconds {60 * 60 * 24};
        int m_flushInterval {3};
        int m_checkEveryN {1024};
        int m_rollSize {1024 * 1024};
    };

    extern LogLevel g_logLevel;
    extern std::mutex g_logMutex;
    extern std::condition_variable g_logCond;
    extern std::vector<LogBuffer::sPtr> g_logBuffers;
}