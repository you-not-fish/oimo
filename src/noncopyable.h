#pragma once

namespace Oimo {
    class noncopyable {
    protected:
        noncopyable() = default;
        ~noncopyable() = default;
        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    };
} // namespace Oimo