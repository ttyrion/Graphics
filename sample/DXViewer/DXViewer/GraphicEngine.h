#pragma once
#include <d3d11.h>

class GraphicEngine
{
public:
    GraphicEngine();
    ~GraphicEngine();

    bool InitDirect3D(HWND render_window, const unsigned int width, const unsigned int height);
    void ReSizeRender(const unsigned int width, const unsigned int height);
    void UpdateScene();
    void DrawScene();

private:
    void UpdateTestScene();

private:
    ID3D11Device* d3d_device_ = NULL;
    ID3D11DeviceContext* d3d_immediate_context_ = NULL;
    IDXGISwapChain* d3d_swap_chain_ = NULL;
    ID3D11Texture2D* d3d_depth_stencil_buffer_ = NULL;
    ID3D11RenderTargetView* d3d_render_target_view_ = NULL;
    ID3D11DepthStencilView* d3d_depth_stencil_view_ = NULL;
    D3D11_VIEWPORT d3d_screen_viewport;
    UINT x4_msaa_uality_ = 0;

    unsigned int width_ = 0;
    unsigned int height_ = 0;
};

