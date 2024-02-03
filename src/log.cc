#include <memory>
#include "log.h"

namespace Oimo {
    LogLevel g_logLevel = LogLevel::INFO;
    
    Logger::Logger(LogLevel level, const char* file, int line, const char* func)
        : m_stream() {
        m_stream << level << " " << file << ":" << line << " " << func << " ";
    }

    Logger::~Logger() {
        m_stream << "\n";
        {
            std::lock_guard<std::mutex> lock(g_logMutex);
            g_logBuffers.push_back(m_stream.buffer());
        }
    }
}