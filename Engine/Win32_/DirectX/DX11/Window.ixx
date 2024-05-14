module;

#include "../../FatWin32_.hpp"

export module DX11Window;

export import Mouse;
export import Keyboard;

#if _MSVC_LANG > 202002L
import std;
#else
#if _MSVC_LANG == 202002L
#ifdef NDEBUG
import std.core;
#else
import <string>;
import <stdexcept>;
#endif // NDEBUG
#else
#error C++20 or greater version required
#endif // _MSVC_LANG == 202002L
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
        fatpound::win32::io::Keyboard kbd;
        fatpound::win32::io::Mouse mouse;


    protected:


    private:
        static LRESULT CALLBACK HandleMsgSetup_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK HandleMsgThunk_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


    private:
        LRESULT HandleMsg_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


    private:
        HWND hWnd_ = nullptr;
        HINSTANCE hInst_ = nullptr;

        static constexpr auto wndClassName_ = L"FatPound DirectX11 Framework Window";
    };
}