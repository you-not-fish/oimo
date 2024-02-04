#include "log.h"
#include "logThread.h"
#include "config.h"
#include "singleton.h"
#include <iostream>
#include "logThread.h"
#include <cassert>
#include <chrono>

namespace Oimo {
    LogThread::LogThread()
        : m_running(false)
    {
    }

    LogThread::~LogThread()
    {
        if (m_running)
        {
            stop();
        }
    }

    void LogThread::start()
    {
        assert(!m_running);
        m_running = true;
        m_thread = std::thread(&LogThread::threadFunc, this);
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock);
    }

    void LogThread::stop()
    {
        m_running = false;
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void LogThread::threadFunc()
    {
        assert(m_running);
        LogFile::uPtr file;
        bool appendToFile = Singleton<Config>::instance().getBool("log.append_to_file");
        bool appendToStdout = Singleton<Config>::instance().getBool("log.append_to_stdout", true);
        if (appendToFile)
        {
            std::string filePath = Singleton<Config>::instance().get("log.file_path", "/tmp/Oimo");
            std::string fileName = Singleton<Config>::instance().get("log.file_prefix", "Oimo");
            file.reset(new LogFile(filePath + "/" + fileName));
        }
        m_cond.notify_one();
        while (m_running)
        {
            m_buffers.clear();
            {
                std::unique_lock<std::mutex> lock(g_logMutex);
                g_logCond.wait_for(lock, std::chrono::seconds(3),
                    [this] { return !g_logBuffers.empty() && this->m_running; });
                m_buffers.swap(g_logBuffers);
            }
            if (appendToFile && file) {
                assert(file);
                for (const auto& buffer : m_buffers) {
                    file->append(buffer->data(), buffer->length());
                }
                file->flush();
            }
            if (appendToStdout) {
                for (const auto& buffer : m_buffers) {
                    fwrite_unlocked(buffer->data(), 1, buffer->length(), stdout);
                }
                fflush(stdout);
            }
        }
    }
} // namespace Oimo
