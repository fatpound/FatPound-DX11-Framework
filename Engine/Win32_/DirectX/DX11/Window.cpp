module;

#include "../../FatWin32_.hpp"

#include <cassert>

module DX11Window;

import DX11Graphics;

namespace fatpound::win32::dx11
{
    Window::Window(HINSTANCE hInst)
        :
        hInst_(hInst)
    {
        try
        {
            WNDCLASSEX wc = {};
            wc.cbSize = sizeof(wc);
            wc.style = CS_CLASSDC;
            wc.lpfnWndProc = &Window::HandleMsgSetup_;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hInstance = hInst_;
            wc.hIcon = nullptr;
            wc.hCursor = nullptr;
            wc.hbrBackground = nullptr;
            wc.lpszMenuName = nullptr;
            wc.lpszClassName = wndClassName_;
            wc.hIconSm = nullptr;
            wc.hIcon = nullptr;
            wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

            RegisterClassEx(&wc);

            RECT wr = {};
            wr.left = 150;
            wr.right = Graphics::ScreenWidth + wr.left;
            wr.top = 150;
            wr.bottom = Graphics::ScreenHeight + wr.top;

            AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

            hWnd_ = CreateWindow(
                wndClassName_,
                L"FatPound DX11 Framework",
                WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                wr.left/*CW_USEDEFAULT*/,
                wr.top/*CW_USEDEFAULT*/,
                wr.right - wr.left,
                wr.bottom - wr.top,
                nullptr,
                nullptr,
                hInst,
                this
            );

            if (hWnd_ == nullptr) [[unlikely]]
            {
                throw std::runtime_error("Could NOT create HWND!");
            }

            ShowWindow(hWnd_, SW_SHOWDEFAULT);
            UpdateWindow(hWnd_);
        }
        catch (const std::exception& ex)
        {
            throw ex;
        }
        catch (...)
        {
            MessageBox(nullptr, L"Non-STD Exception was thrown inside Window CTOR!", L"Window Error", MB_OK | MB_ICONERROR);

            throw;
        }
    }
    Window::~Window()
    {
        UnregisterClass(wndClassName_, hInst_);
    }

    HWND Window::GetHwnd() const noexcept
    {
        return hWnd_;
    }

    bool Window::ProcessMessage()
    {
        MSG msg;

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) [[unlikely]]
            {
                return false;
            }
        }

        return true;
    }
    bool Window::IsActive() const
    {
        return GetActiveWindow() == hWnd_;
    }
    bool Window::IsMinimized() const
    {
        return IsIconic(hWnd_) != 0;
    }

    void Window::Kill()
    {
        PostQuitMessage(0);
    }

    LRESULT WINAPI Window::HandleMsgSetup_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_NCCREATE)
        {
            const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
            Window* const pWnd = reinterpret_cast<Window*>(pCreate->lpCreateParams);

            assert(pWnd != nullptr);

            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
            SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk_));

            return pWnd->HandleMsg_(hWnd, msg, wParam, lParam);
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    LRESULT WINAPI Window::HandleMsgThunk_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {

        Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        return pWnd->HandleMsg_(hWnd, msg, wParam, lParam);
    }
    LRESULT Window::HandleMsg_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_KILLFOCUS:
            kbd.ClearKeyStateBitset_();
            break;

            // ************ KEYBOARD MESSAGES ************ //
        case WM_KEYDOWN:
            if (!(lParam & 0x40000000) || kbd.AutoRepeatIsEnabled()) // no thank you on the autorepeat
            {
                kbd.OnKeyPressed_(static_cast<unsigned char>(wParam));
            }
            break;

        case WM_KEYUP:
            kbd.OnKeyReleased_(static_cast<unsigned char>(wParam));
            break;

        case WM_CHAR:
            kbd.OnChar_(static_cast<unsigned char>(wParam));
            break;
            // ************ END KEYBOARD MESSAGES ************ //

            // ************ MOUSE MESSAGES ************ //
        case WM_MOUSEMOVE:
        {
            POINTS pt = MAKEPOINTS(lParam);

            if (pt.x > 0 && pt.x < Graphics::ScreenWidth && pt.y > 0 && pt.y < Graphics::ScreenHeight)
            {
                mouse.OnMouseMove_(pt.x, pt.y);

                if (!mouse.IsInWindow())
                {
                    SetCapture(hWnd);

                    mouse.OnMouseEnter_();
                }
            }
            else
            {
                if (wParam & (MK_LBUTTON | MK_RBUTTON))
                {
                    pt.x = std::max(static_cast<short int>(0), pt.x);
                    pt.x = std::min(static_cast<short int>(Graphics::ScreenWidth - 1), pt.x);
                    pt.y = std::max(static_cast<short int>(0), pt.y);
                    pt.y = std::min(static_cast<short int>(Graphics::ScreenHeight - 1), pt.y);

                    mouse.OnMouseMove_(pt.x, pt.y);
                }
                else
                {
                    ReleaseCapture();

                    mouse.OnMouseLeave_();
                    mouse.OnLeftReleased_();
                    mouse.OnRightReleased_();
                }
            }
            
        }
            break;

        case WM_LBUTTONDOWN:
            mouse.OnLeftPressed_();
            SetForegroundWindow(hWnd);
            break;

        case WM_RBUTTONDOWN:
            mouse.OnRightPressed_();
            break;

        case WM_LBUTTONUP:
            mouse.OnLeftReleased_();
            break;

        case WM_RBUTTONUP:
            mouse.OnRightReleased_();
            break;
        
        case WM_MOUSEWHEEL:
        {
            if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            {
                mouse.OnWheelUp_();
            }
            else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
            {
                mouse.OnWheelDown_();
            }
            
        }
            break;
        // ************ END MOUSE MESSAGES ************ //
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}