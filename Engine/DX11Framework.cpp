module;

#include "Win32_/FatWin32_.hpp"

module DX11Framework;

namespace fatpound::win32::dx11
{
    Framework::Framework(HINSTANCE hInst)
        try
        :
        wnd_(hInst),
        gfx_(wnd_.GetHwnd())
    {

    }
    catch (const std::exception& ex)
    {
        throw ex;
    }
    catch (...)
    {
        MessageBox(nullptr, L"Non-STD Exception was thrown when entering Framework CTOR!", L"Framework Error", MB_OK | MB_ICONERROR);

        throw;
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