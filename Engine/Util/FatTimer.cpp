module;

#if _MSVC_LANG == 202002L
#ifndef NDEBUG
#include <chrono>
#endif // !NDEBUG
#else
#if _MSVC_LANG < 202002L
#error C++20 or greater version required
#endif // _MSVC_LANG < 202002L
#endif // _MSVC_LANG == 202002L

module FatTimer;

namespace fatpound::util
{
    float Timer::Mark() noexcept
    {
        const auto old = last_;
        last_ = std::chrono::steady_clock::now();

        const std::chrono::duration<float> frameTime = last_ - old;

        return frameTime.count();
    }
    float Timer::Peek() const noexcept
    {
        return std::chrono::duration<float>(std::chrono::steady_clock::now() - last_).count();
    }
}