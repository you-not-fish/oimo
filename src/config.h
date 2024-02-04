#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <map>
#include <yaml-cpp/yaml.h>

namespace Oimo {
    class Config {
    public:
        using sPtr = std::shared_ptr<Config>;
        using Map = std::map<std::string, std::string>;

        Config();
        ~Config();

        void load(const std::string& filename);
        void set(const std::string& key, const std::string& value);
        void set(const std::string& key, const char* value);
        void set(const std::string& key, int value);
        void set(const std::string& key, float value);
        void set(const std::string& key, double value);
        void set(const std::string& key, bool value);

        std::string get(const std::string& key, const std::string& defaultValue = "") const;
        int getInt(const std::string& key, int defaultValue = 0) const;
        float getFloat(const std::string& key, float defaultValue = 0.0f) const;
        double getDouble(const std::string& key, double defaultValue = 0.0) const;
        bool getBool(const std::string& key, bool defaultValue = false) const;

        bool has(const std::string& key) const;
        void remove(const std::string& key);

        const Map& map() const;

    private:
        void loadToMap(const std::string& prefix, const YAML::Node& node);
        Map m_map;
        mutable std::mutex m_mutex;
    };
}