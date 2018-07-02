#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#define SCREEN_DEPTH  1000.0f
#define SCREEN_NEAR  0.1f

typedef struct
{
    DirectX::XMFLOAT3    object_coordinate;  //标物体表面的坐标
    float       rhw;
    DirectX::XMFLOAT4    diffuse;
    DirectX::XMFLOAT2       texture_coordinate;  //纹理坐标
} VIDEO_VERTEX;

typedef struct
{
    DirectX::XMFLOAT3    position;  //x, y, z
    DirectX::XMFLOAT4    color;     //r, g, b, a
} SIMPLE_VERTEX;

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
    D3D11_DEPTH_STENCIL_DESC depth_stenci_desc_;
    ID3D11DepthStencilState* depth_stenci_state_ = NULL;
    D3D11_DEPTH_STENCIL_DESC disable_depth_stenci_desc_;
    ID3D11DepthStencilState* disable_depth_stenci_state_ = NULL;
    ID3D11Texture2D* d3d_depth_stencil_buffer_ = NULL;
    ID3D11RenderTargetView* d3d_render_target_view_ = NULL;
    ID3D11DepthStencilView* d3d_depth_stencil_view_ = NULL;
    D3D11_VIEWPORT d3d_screen_viewport;
    D3D11_RASTERIZER_DESC raster_desc_;
    ID3D11RasterizerState* raster_state_ = NULL;
    UINT x4_msaa_uality_ = 0;
    ID3D11Buffer* vertex_buffer_ = NULL;
    DirectX::XMMATRIX orthographic_matrix_;
    DirectX::XMMATRIX world_matrix_;

    unsigned int width_ = 0;
    unsigned int height_ = 0;
};

