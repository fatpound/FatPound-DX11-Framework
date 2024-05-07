module;

#include "../../FatWin32_.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>

#include <immintrin.h>

#include <cassert>

#define MSAA_QUALITY 1u

#pragma comment(lib, "d3d11")
#pragma comment(lib, "D3DCompiler")

module DX11Graphics;

using fatpound::util::Color;

namespace wrl = Microsoft::WRL;

namespace fatpound::win32::dx11
{
    Graphics::Graphics(HWND hWnd)
    {
        assert(hWnd != nullptr);

        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1u;
        sd.BufferDesc.Width = Graphics::ScreenWidth;
        sd.BufferDesc.Height = Graphics::ScreenHeight;
        sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 0u;
        sd.BufferDesc.RefreshRate.Denominator = 0u;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = MSAA_QUALITY;
        sd.SampleDesc.Quality = 0u;
        sd.Windowed = TRUE;

#ifdef NDEBUG
        constexpr UINT swapCreateFlags = 0u;
#else
        constexpr UINT swapCreateFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

        HRESULT hr;

        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            swapCreateFlags,
            nullptr,
            0u,
            D3D11_SDK_VERSION,
            &sd,
            &pSwapChain_,
            &pDevice_,
            nullptr,
            &pImmediateContext_
        );

        if (FAILED(hr)) [[unlikely]]
        {
            throw std::runtime_error("Could NOT create Device and SwapChain!");
        }

        wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;

        hr = pSwapChain_->GetBuffer(0u, __uuidof(ID3D11Resource), &pBackBuffer);

        if (FAILED(hr)) [[unlikely]]
        {
            throw std::runtime_error("Could NOT get the backbuffer pointer from SwapChain!");
        }

        hr = pDevice_->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRenderTargetView_);

        if (FAILED(hr)) [[unlikely]]
        {
            throw std::runtime_error("Could NOT create RenterTargetView!");
        }

        pImmediateContext_->OMSetRenderTargets(1u, pRenderTargetView_.GetAddressOf(), nullptr);

        D3D11_VIEWPORT vp = {};
        vp.Width = static_cast<float>(Graphics::ScreenWidth);
        vp.Height = static_cast<float>(Graphics::ScreenHeight);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;

        pImmediateContext_->RSSetViewports(1, &vp);

        D3D11_TEXTURE2D_DESC sysTexDesc = {};
        sysTexDesc.Width = Graphics::ScreenWidth;
        sysTexDesc.Height = Graphics::ScreenHeight;
        sysTexDesc.MipLevels = 1u;
        sysTexDesc.ArraySize = 1u;
        sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        sysTexDesc.SampleDesc.Count = MSAA_QUALITY;
        sysTexDesc.SampleDesc.Quality = 0u;
        sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
        sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        sysTexDesc.MiscFlags = 0u;

        hr = pDevice_->CreateTexture2D(&sysTexDesc, nullptr, &pSysBufferTexture_);

        if (FAILED(hr)) [[unlikely]]
        {
            throw std::runtime_error("Could NOT create Texture2D!");
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = sysTexDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1u;

        hr = pDevice_->CreateShaderResourceView(pSysBufferTexture_.Get(), &srvDesc, &pSysBufferTextureView_);

        if (FAILED(hr)) [[unlikely]]
        {
            throw std::runtime_error("Could NOT create ShaderResourceView!");
        }

        wrl::ComPtr<ID3DBlob> pBlob = nullptr;

        D3DReadFileToBlob(L"PSFrameBuffer.cso", &pBlob);

        hr = pDevice_->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader_);

        if (FAILED(hr)) [[unlikely]]
        {
            throw std::runtime_error("Could NOT create PixelShader!");
        }

        pBlob.Reset(); // Now using the pBlob for Vertex Shader output
        D3DReadFileToBlob(L"VSFrameBuffer.cso", &pBlob);

        hr = pDevice_->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader_);

        if (FAILED(hr)) [[unlikely]]
        {
            throw std::runtime_error("Could NOT create VertexShader!");
        }

        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(FullScreenQuad_::Vertex) * 6u;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0u;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = reinterpret_cast<void*>(const_cast<FullScreenQuad_::Vertex*>(FullScreenQuad_::vertices.data()));

        hr = pDevice_->CreateBuffer(&bd, &initData, &pVertexBuffer_);

        if (FAILED(hr)) [[unlikely]]
        {
            throw std::runtime_error("Could NOT create Subresource Buffer!");
        }

        constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 2> ied =
        {
            {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
            }
        };

        hr = pDevice_->CreateInputLayout(ied.data(), 2u, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout_);

        if (FAILED(hr)) [[unlikely]]
        {
            throw std::runtime_error("Could NOT create InputLayout!");
        }

        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0.0f;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = pDevice_->CreateSamplerState(&sampDesc, &pSamplerState_);

        if (FAILED(hr)) [[unlikely]]
        {
            throw std::runtime_error("Could NOT create SamplerState");
        }

        pSysBuffer_ = reinterpret_cast<Color*>(_aligned_malloc(sizeof(Color) * Graphics::ScreenWidth * Graphics::ScreenHeight, 16u));

        pImmediateContext_->IASetInputLayout(pInputLayout_.Get());
        pImmediateContext_->VSSetShader(pVertexShader_.Get(), nullptr, 0u);
        pImmediateContext_->PSSetShader(pPixelShader_.Get(), nullptr, 0u);
        pImmediateContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        constexpr UINT stride = sizeof(FullScreenQuad_::Vertex);
        constexpr UINT offset = 0u;

        pImmediateContext_->IASetVertexBuffers(0u, 1u, pVertexBuffer_.GetAddressOf(), &stride, &offset);
        pImmediateContext_->PSSetShaderResources(0u, 1u, pSysBufferTextureView_.GetAddressOf());
        pImmediateContext_->PSSetSamplers(0u, 1u, pSamplerState_.GetAddressOf());
    }
    Graphics::~Graphics()
    {
        if (pSysBuffer_ != nullptr) [[likely]]
        {
            _aligned_free(pSysBuffer_);
            pSysBuffer_ = nullptr;
        }

        if (pImmediateContext_ != nullptr) [[likely]]
        {
            pImmediateContext_->ClearState();
        }
    }

    void Graphics::BeginFrame()
    {
        std::memset(pSysBuffer_, 0u, sizeof(Color) * Graphics::ScreenHeight * Graphics::ScreenWidth);
    }
    void Graphics::EndFrame()
    {
        HRESULT hr = pImmediateContext_->Map(pSysBufferTexture_.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedSysBufferTexture_);

        if (FAILED(hr)) [[unlikely]]
        {
            throw std::exception("Could NOT Map the ImmediateContext!");
        }
        
        Color* pDst = reinterpret_cast<Color*>(mappedSysBufferTexture_.pData);
        
        const std::size_t dstPitch = mappedSysBufferTexture_.RowPitch / sizeof(Color);
        constexpr std::size_t srcPitch = Graphics::ScreenWidth;
        constexpr std::size_t rowBytes = srcPitch * sizeof(Color);
        
        for (std::size_t y = 0u; y < Graphics::ScreenHeight; ++y)
        {
            std::memcpy(&pDst[y * dstPitch], &pSysBuffer_[y * srcPitch], rowBytes);
        }
        
        pImmediateContext_->Unmap(pSysBufferTexture_.Get(), 0u);
        pImmediateContext_->Draw(6u, 0u);
        
        hr = pSwapChain_->Present(1u, 0u);
        
        if (FAILED(hr)) [[unlikely]]
        {
            throw std::exception("SwapChain could NOT Present and Draw its contents!");;
        }
    }
    void Graphics::PutPixel(int x, int y, Color color)
    {
        assert(x >= 0);
        assert(x < Graphics::ScreenWidth);
        assert(y >= 0);
        assert(y < Graphics::ScreenHeight);

        pSysBuffer_[Graphics::ScreenWidth * y + x] = color;
    }
}