module;

#include "Win32_/FatWin32_.hpp"

export module DX11Framework;

import DX11Window;
import DX11Graphics;

#if _MSVC_LANG > 202002L
import std;
#elif _MSVC_LANG == 202002L
import std.core;
#else
#error MSVC /std:c++20 or newer option required
#endif // _MSVC_LANG > 202002L

export namespace fatpound::win32::dx11
{
    class Framework final
    {
    public:
        Framework(HINSTANCE hInst);

        Framework() = delete;
        Framework(const Framework& src) = delete;
        Framework& operator = (const Framework& src) = delete;
        Framework(Framework&& src) = delete;
        Framework& operator = (Framework&& src) = delete;
        ~Framework() = default;


    public:
        void Run();


    protected:


    private:
        void ComposeFrame_();


    private:
        Window wnd_;
        Graphics gfx_;
    };
}