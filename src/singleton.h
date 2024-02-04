#pragma once
namespace Oimo {
    template <typename T>
    class Singleton {
    public:
        static T& instance()
        {
            static T instance;
            return instance;
        }

    protected:
        Singleton() = default;
        ~Singleton() = default;
    };
}