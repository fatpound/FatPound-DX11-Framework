module;

module AutoTimer;

namespace fatpound::util
{
    float AutoTimer::Mark() noexcept
    {
        const auto old = last_;

        last_ = std::chrono::steady_clock::now();

        const std::chrono::duration<float> frameTime = last_ - old;

        return frameTime.count();
    }
    float AutoTimer::Peek() const noexcept
    {
        const auto& now = std::chrono::steady_clock::now();

        return std::chrono::duration<float>(now - last_).count();
    }
}