module;

#include "../FatWin32_.hpp"

#if _MSVC_LANG == 202002L
#ifndef NDEBUG
#include <queue>
#include <optional>
#endif // !NDEBUG
#else
#if _MSVC_LANG < 202002L
#error C++20 or greater version required
#endif // _MSVC_LANG < 202002L
#endif // _MSVC_LANG == 202002L

module Mouse;

namespace fatpound::win32::io
{
    auto Mouse::GetPos() const noexcept -> std::pair<int, int>
    {
        return { x_, y_ };
    }

    auto Mouse::ReadRawDelta() noexcept -> std::optional<RawDelta>
    {
        if (rawDeltaBuffer_.empty())
        {
            return std::nullopt;
        }

        const RawDelta delta = rawDeltaBuffer_.front();
        rawDeltaBuffer_.pop();

        return delta;
    }

    auto Mouse::ReadFromBuffer() noexcept -> Event
    {
        if (buffer_.size() > 0u)
        {
            Event e = buffer_.front();
            buffer_.pop();

            return e;
        }

        return {};
    }

    int Mouse::GetPosX() const noexcept
    {
        return x_;
    }
    int Mouse::GetPosY() const noexcept
    {
        return y_;
    }

    bool Mouse::IsInWindow() const noexcept
    {
        return isInWindow_;
    }
    bool Mouse::LeftIsPressed() const noexcept
    {
        return leftIsPressed_;
    }
    bool Mouse::RightIsPressed() const noexcept
    {
        return rightIsPressed_;
    }
    bool Mouse::WheelIsPressed() const noexcept
    {
        return wheelIsPressed_;
    }
    bool Mouse::BufferIsEmpty() const noexcept
    {
        return buffer_.empty();
    }

    void Mouse::FlushBuffer() noexcept
    {
        buffer_ = std::queue<Event>();
    }

    void Mouse::OnMouseMove_(int x, int y) noexcept
    {
        x_ = x;
        y_ = y;

        buffer_.push(Mouse::Event(Mouse::Event::Type::Move, *this));

        TrimBuffer_();
    }
    void Mouse::OnMouseEnter_() noexcept
    {
        isInWindow_ = true;

        buffer_.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
        TrimBuffer_();
    }
    void Mouse::OnMouseLeave_() noexcept
    {
        isInWindow_ = false;

        buffer_.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
        TrimBuffer_();
    }
    void Mouse::OnRawDelta_(int dx, int dy) noexcept
    {
        rawDeltaBuffer_.push(Mouse::RawDelta(dx, dy));

        TrimBuffer_();
    }
    void Mouse::OnLeftPressed_() noexcept
    {
        leftIsPressed_ = true;

        buffer_.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
        TrimBuffer_();
    }
    void Mouse::OnLeftReleased_() noexcept
    {
        leftIsPressed_ = false;

        buffer_.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
        TrimBuffer_();
    }
    void Mouse::OnRightPressed_() noexcept
    {
        rightIsPressed_ = true;

        buffer_.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
        TrimBuffer_();
    }
    void Mouse::OnRightReleased_() noexcept
    {
        rightIsPressed_ = false;

        buffer_.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
        TrimBuffer_();
    }
    void Mouse::OnWheelPressed_() noexcept
    {
        wheelIsPressed_ = true;

        buffer_.push(Mouse::Event(Mouse::Event::Type::WheelPress, *this));
        TrimBuffer_();
    }
    void Mouse::OnWheelReleased_() noexcept
    {
        wheelIsPressed_ = false;

        buffer_.push(Mouse::Event(Mouse::Event::Type::WheelRelease, *this));
        TrimBuffer_();
    }
    void Mouse::OnWheelUp_() noexcept
    {
        buffer_.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
        TrimBuffer_();
    }
    void Mouse::OnWheelDown_() noexcept
    {
        buffer_.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
        TrimBuffer_();
    }
    void Mouse::OnWheelDelta_(int delta) noexcept
    {
        wheelDeltaCarry_ += delta;

        while (wheelDeltaCarry_ >= WHEEL_DELTA)
        {
            wheelDeltaCarry_ -= WHEEL_DELTA;
            OnWheelUp_();
        }

        while (wheelDeltaCarry_ <= -WHEEL_DELTA)
        {
            wheelDeltaCarry_ += WHEEL_DELTA;
            OnWheelDown_();
        }
    }
    void Mouse::TrimBuffer_() noexcept
    {
        while (buffer_.size() > bufferSize_)
        {
            buffer_.pop();
        }
    }
    void Mouse::TrimRawInputBuffer_() noexcept
    {
        while (rawDeltaBuffer_.size() > bufferSize_)
        {
            rawDeltaBuffer_.pop();
        }
    }


    // Event

    Mouse::Event::Event() noexcept
        :
        type_(Type::Invalid),
        leftIsPressed_(false),
        rightIsPressed_(false),
        wheelIsPressed_(false),
        x_(0),
        y_(0)
    {

    }
    Mouse::Event::Event(Type type, const Mouse& parent) noexcept
        :
        type_(type),
        leftIsPressed_(parent.leftIsPressed_),
        rightIsPressed_(parent.rightIsPressed_),
        wheelIsPressed_(parent.wheelIsPressed_),
        x_(parent.x_),
        y_(parent.y_)
    {

    }

    auto Mouse::Event::GetPos() const noexcept -> std::pair<int, int>
    {
        return { x_, y_ };
    }

    auto Mouse::Event::GetType() const noexcept -> Type
    {
        return type_;
    }

    int Mouse::Event::GetPosX() const noexcept
    {
        return x_;
    }
    int Mouse::Event::GetPosY() const noexcept
    {
        return y_;
    }

    bool Mouse::Event::IsValid() const noexcept
    {
        return type_ != Event::Type::Invalid;
    }
    bool Mouse::Event::LeftIsPressed() const noexcept
    {
        return leftIsPressed_;
    }
    bool Mouse::Event::RightIsPressed() const noexcept
    {
        return rightIsPressed_;
    }
    bool Mouse::Event::WheelIsPressed() const noexcept
    {
        return wheelIsPressed_;
    }
}