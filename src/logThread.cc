#include "log.h"
#include "logThread.h"
#include "config.h"
#include "singleton.h"
#include <iostream>
#include "logThread.h"
#include <functional>
#include <cassert>
#include <chrono>

namespace Oimo {
    LogThread::LogThread()
        : m_running(false),
        m_thread(std::bind(&LogThread::threadFunc, this), "LogThread"),
        m_appendToFile(Singleton<Config>::instance().getBool("log.append_to_file")),
        m_appendToStdout(Singleton<Config>::instance().getBool("log.append_to_stdout", true))
    {
        if (m_appendToFile)
        {
            std::string filePath = Singleton<Config>::instance().get("log.file_path", "/tmp/Oimo");
            std::string fileName = Singleton<Config>::instance().get("log.file_prefix", "Oimo");
            m_file.reset(new LogFile(filePath + "/" + fileName));
        }
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
        m_thread.start();
    }

    void LogThread::stop()
    {
        m_running = false;
        m_thread.join();
    }

    void LogThread::threadFunc()
    {
        while (m_running)
        {
            m_buffers.clear();
            {
                std::unique_lock<std::mutex> lock(g_logMutex);
                g_logCond.wait_for(lock, std::chrono::seconds(3),
                    [this] { return !g_logBuffers.empty() && this->m_running; });
                m_buffers.swap(g_logBuffers);
            }
            if (m_appendToFile) {
                assert(m_file);
                for (const auto& buffer : m_buffers) {
                    m_file->append(buffer->data(), buffer->length());
                }
                m_file->flush();
            }
            if (m_appendToStdout) {
                for (const auto& buffer : m_buffers) {
                    fwrite_unlocked(buffer->data(), 1, buffer->length(), stdout);
                }
                fflush(stdout);
            }
        }
    }
} // namespace Oimo
