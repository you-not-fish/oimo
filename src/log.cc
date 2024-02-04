#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <cstdio>
#include <errno.h>
#include "log.h"

namespace Oimo {
    LogLevel g_logLevel = LogLevel::INFO;
    std::mutex g_logMutex;
    std::condition_variable g_logCond;
    std::vector<LogBuffer::sPtr> g_logBuffers;
    
    Logger::Logger(LogLevel level, const char* file, int line, const char* func)
        : m_stream() {
        logTime();
        m_stream << " [" << logLevelToString(level) << "] ";
        m_stream << file << ":" << line << " " << func << " ";
    }

    Logger::~Logger() {
        m_stream << "\n";
        {
            std::lock_guard<std::mutex> lock(g_logMutex);
            g_logBuffers.push_back(m_stream.buffer());
            if (g_logBuffers.size() > 10) {
                g_logCond.notify_one();
            }
            // std::cout << m_stream.buffer()->toString();
        }
    }

    LogLevel Logger::logLevel() {
        return g_logLevel;
    }

    void Logger::setLogLevel(LogLevel level) {
        g_logLevel = level;
    }

    void Logger::logTime() {
        time_t now = time(nullptr);
        struct tm* t = localtime(&now);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
        m_stream << buf;
    }

    LogFile::LogFile(const std::string& fileName)
        : m_fileName(fileName),
          m_lastFlushTime(0),
          m_lastRollTime(0),
          m_writtenBytes(0) {
        time_t now = time(nullptr);
        std::string newFileName = formatFileName(now);
        m_fp = fopen(newFileName.c_str(), "ae");
        if (!m_fp) {
            fprintf(stderr, "LogFile::LogFile() failed %s\n", strerror(errno));
        }
    }

    LogFile::~LogFile() {
        fclose(m_fp);
    }

    void LogFile::append(const char* data, int len) {
        writeToFile(data, len);
        if (m_writtenBytes > m_rollSize) {
            rollFile();
        } else {
            time_t now = time(nullptr);
            if (now - m_lastFlushTime > m_flushInterval) {
                flush();
            }
            ++m_count;
            if (m_count >= m_checkEveryN) {
                m_count = 0;
                time_t thisPeriod = now / m_rollPerSeconds * m_rollPerSeconds;
                if (thisPeriod != m_startOfPeriod) {
                    rollFile();
                }
            }
            
        }
    }

    void LogFile::writeToFile(const char* data, int len) {
        size_t n = fwrite_unlocked(data, 1, len, m_fp);
        size_t remain = len - n;
        while (remain > 0) {
            size_t x = fwrite_unlocked(data + n, 1, remain, m_fp);
            if (x == 0) {
                int err = ferror(m_fp);
                if (err) {
                    fprintf(stderr, "LogFile::append() failed %s\n", strerror(err));
                }
                break;
            }
            n += x;
            remain = len - n;
        }
        m_writtenBytes += len;
    }

    void LogFile::flush() {
        fflush(m_fp);
        m_lastFlushTime = time(nullptr);
    }

    std::string LogFile::formatFileName(time_t now) {
        std::string fileName = m_fileName;
        std::string newFileName;
        newFileName = fileName;
        char timebuf[32];
        strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S.", localtime(&now));
        newFileName += timebuf;
        newFileName += "log";
        return newFileName;
    }

    void LogFile::rollFile() {
        time_t now = time(nullptr);
        std::string fileName = formatFileName(now);
        fclose(m_fp);
        m_fp = fopen(fileName.c_str(), "ae");
        m_writtenBytes = 0;
        m_lastRollTime = now;
        m_startOfPeriod = now / m_rollPerSeconds * m_rollPerSeconds;
    }


}