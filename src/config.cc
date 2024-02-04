#include "config.h"
namespace Oimo {
    Config::Config()
    {
        //get filename from env
        char* filename = std::getenv("OIMO_CONFIG");
        if (filename)
        {
            load(filename);
        }
        else
        {
            load("config.yaml");
        }
    }

    Config::~Config()
    {
    }

    void Config::load(const std::string& filename)
    {
        fprintf(stdout, "Loading config from %s\n", filename.c_str());
        std::lock_guard<std::mutex> lock(m_mutex);
        m_map.clear();
        YAML::Node node = YAML::LoadFile(filename);
        loadToMap("", node);
    }

    void Config::loadToMap(const std::string& prefix, const YAML::Node& node)
    {
        if (node.IsScalar())
        {
            m_map[prefix] = node.as<std::string>();
            return;
        }
        else if (node.IsMap())
        {
            for (auto it = node.begin(); it != node.end(); ++it)
            {
                std::string key = it->first.as<std::string>();
                if (prefix.empty())
                {
                    loadToMap(key, it->second);
                }
                else
                {
                    loadToMap(prefix + "." + key, it->second);
                }
            }
        }
        else if (node.IsSequence())
        {
            m_map[prefix + ".length"] = std::to_string(node.size());
            for (std::size_t i = 0; i < node.size(); ++i)
            {
                loadToMap(prefix + "[" + std::to_string(i) + "]", node[i]);
            }
        }
        else
        {
            fprintf(stderr, "Invalid node type\n");
        }
    }

    void Config::set(const std::string& key, const std::string& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_map[key] = value;
    }

    void Config::set(const std::string& key, const char* value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_map[key] = value;
    }

    void Config::set(const std::string& key, int value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_map[key] = std::to_string(value);
    }

    void Config::set(const std::string& key, float value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_map[key] = std::to_string(value);
    }

    void Config::set(const std::string& key, double value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_map[key] = std::to_string(value);
    }

    void Config::set(const std::string& key, bool value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_map[key] = value ? "true" : "false";
    }

    std::string Config::get(const std::string& key, const std::string& defaultValue) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end())
        {
            return it->second;
        }
        return defaultValue;
    }

    int Config::getInt(const std::string& key, int defaultValue) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end())
        {
            return std::stoi(it->second);
        }
        return defaultValue;
    }

    float Config::getFloat(const std::string& key, float defaultValue) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end())
        {
            return std::stof(it->second);
        }
        return defaultValue;
    }

    double Config::getDouble(const std::string& key, double defaultValue) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end())
        {
            return std::stod(it->second);
        }
        return defaultValue;
    }

    bool Config::getBool(const std::string& key, bool defaultValue) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end())
        {
            return it->second == "true";
        }
        return defaultValue;
    }

    bool Config::has(const std::string& key) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_map.find(key) != m_map.end();
    }

    void Config::remove(const std::string& key)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_map.erase(key);
    }

    const Config::Map& Config::map() const
    {
        return m_map;
    }
} // namespace Oimo