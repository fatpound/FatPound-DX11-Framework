module;

#include "../../FatWin32_.hpp"

#include <d3d11.h>
#include <wrl.h>

#if _MSVC_LANG == 202002L
#ifndef NDEBUG
#include <array>
#endif // !NDEBUG
#endif // _MSVC_LANG == 202002L

export module DX11Graphics;

import FatColor;

#if _MSVC_LANG > 202002L
import std;
#else
#if _MSVC_LANG == 202002L
#ifdef NDEBUG
import std.core;
#else
import <string>;
import <array>;
import <stdexcept>;
//
import <new>;
#endif // NDEBUG
#else
#error C++20 or greater version required
#endif // _MSVC_LANG == 202002L
#endif // _MSVC_LANG > 202002L

export namespace fatpound::win32::dx11
{
    class Graphics final
    {
    public:
        Graphics(HWND hWnd);

        Graphics() = delete;
        Graphics(const Graphics& src) = delete;
        Graphics& operator = (const Graphics& src) = delete;
        Graphics(Graphics&& src) = delete;
        Graphics& operator = (Graphics&& src) = delete;
        ~Graphics();


    public:
        void BeginFrame();
        void EndFrame();
        void PutPixel(int x, int y, fatpound::util::Color color);


    public:
        static constexpr auto ScreenWidth  = 800u;
        static constexpr auto ScreenHeight = 600u;


    protected:


    private:
        struct FullScreenQuad_ final
        {
            struct Vertex final
            {
                float x, y, z;
                float u, v;
            };

            static constexpr std::array<Vertex, 6> vertices =
            {
                Vertex{ -1.0f,  1.0f,  0.5f,  0.0f,  0.0f },
                Vertex{  1.0f,  1.0f,  0.5f,  1.0f,  0.0f },
                Vertex{  1.0f, -1.0f,  0.5f,  1.0f,  1.0f },
                Vertex{ -1.0f,  1.0f,  0.5f,  0.0f,  0.0f },
                Vertex{  1.0f, -1.0f,  0.5f,  1.0f,  1.0f },
                Vertex{ -1.0f, -1.0f,  0.5f,  0.0f,  1.0f }
            };
        };


    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain>              pSwapChain_                = nullptr;
        Microsoft::WRL::ComPtr<ID3D11Device>                pDevice_                   = nullptr;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>         pImmediateContext_         = nullptr;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>      pRenderTargetView_         = nullptr;
        Microsoft::WRL::ComPtr<ID3D11Texture2D>             pSysBufferTexture_         = nullptr;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    pSysBufferTextureView_     = nullptr;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           pPixelShader_              = nullptr;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>          pVertexShader_             = nullptr;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                pVertexBuffer_             = nullptr;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>           pInputLayout_              = nullptr;
        Microsoft::WRL::ComPtr<ID3D11SamplerState>          pSamplerState_             = nullptr;

        D3D11_MAPPED_SUBRESOURCE                            mappedSysBufferTexture_    = {};

        fatpound::util::Color* pSysBuffer_                                             = nullptr;
    };
}