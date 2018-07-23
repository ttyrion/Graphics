#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "Graphic.h"
#include "utility.h"
#include "FPSCamera.h"

#define SCREEN_DEPTH  1000.0f
#define SCREEN_NEAR  0.1f

class GraphicEngine
{
public:
    GraphicEngine();
    ~GraphicEngine();

    bool InitDirect3D(HWND render_window, const unsigned int width, const unsigned int height);
    void ReleaseDirect3D();
    void ReSizeRender(const unsigned int width, const unsigned int height);
    void UpdateScene();
    void DrawScene();

    ID3D11Device* GetDevice() { return d3d_device_; };
    ID3D11DeviceContext* GetDeviceContext() { return d3d_immediate_context_; };

private:
    void UpdateTestScene();
    void Release();

private:
    ID3D11Device* d3d_device_ = NULL;
    ID3D11DeviceContext* d3d_immediate_context_ = NULL;
    IDXGISwapChain* d3d_swap_chain_ = NULL;
    D3D11_DEPTH_STENCIL_DESC depth_stenci_desc_;
    ID3D11DepthStencilState* depth_stenci_state_ = NULL;
    D3D11_DEPTH_STENCIL_DESC disable_depth_stenci_desc_;
    ID3D11DepthStencilState* disable_depth_stenci_state_ = NULL;
    ID3D11Texture2D* d3d_depth_stencil_buffer_ = NULL;
    ID3D11RenderTargetView* d3d_render_target_view_ = NULL;
    ID3D11DepthStencilView* d3d_depth_stencil_view_ = NULL;
    D3D11_VIEWPORT d3d_screen_viewport_;
    D3D11_RASTERIZER_DESC raster_desc_;
    ID3D11RasterizerState* raster_state_ = NULL;
    UINT x4_msaa_uality_ = 0;

    Graphic* graphic_ = nullptr;
    //ColorCamera camera_;
    //CubeCamera camera_;
    FPSCamera camera_;
    unsigned int width_ = 0;
    unsigned int height_ = 0;
};

