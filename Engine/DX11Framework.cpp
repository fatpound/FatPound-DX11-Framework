module;

#include "Win32_/FatWin32_.hpp"

module DX11Framework;

namespace fatpound::win32::dx11
{
    Framework::Framework(HINSTANCE hInst)
        :
        wnd_(hInst),
        gfx_(wnd_.GetHwnd())
    {

    }

    void Framework::Run()
    {
        while (wnd_.ProcessMessage())
        {
            gfx_.BeginFrame();
            ComposeFrame_();
            gfx_.EndFrame();
        }
    }

    void Framework::ComposeFrame_()
    {
        
    }
}