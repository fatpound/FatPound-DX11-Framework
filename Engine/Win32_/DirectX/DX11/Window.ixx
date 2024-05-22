module;

#include "../../FatWin32_.hpp"

export module DX11Window;

export import Mouse;
export import Keyboard;

#if _MSVC_LANG > 202002L
import std;
#elif _MSVC_LANG == 202002L
import std.core;
#else
#error MSVC /std:c++20 or newer option required
#endif // _MSVC_LANG > 202002L

export namespace fatpound::win32::dx11
{
    class Window final
    {
    public:
        Window(HINSTANCE hInst);

        Window() = delete;
        Window(const Window& src) = delete;
        Window& operator = (const Window& src) = delete;
        Window(Window&& src) = delete;
        Window& operator = (Window&& src) = delete;
        ~Window();


    public:
        HWND GetHwnd() const noexcept;

        bool ProcessMessage();
        bool IsActive() const;
        bool IsMinimized() const;

        void Kill();


    public:
        NAMESPACE_IO::Keyboard kbd;
        NAMESPACE_IO::Mouse mouse;


    protected:


    private:
        static LRESULT CALLBACK HandleMsgSetup_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK HandleMsgThunk_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


    private:
        LRESULT HandleMsg_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


    private:
        HWND hWnd_ = nullptr;
        HINSTANCE hInst_ = nullptr;

        static constexpr bool cursor_enabled_ = true;

        static constexpr auto title_        = L"FatPound DX11 Framework";
        static constexpr auto wndClassName_ = L"FatPound DirectX11 Framework Window";
    };
}