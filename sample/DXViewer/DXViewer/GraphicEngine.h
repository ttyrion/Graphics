#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "Graphic.h"
#include "ColorCamera.h"

#define SCREEN_DEPTH  1000.0f
#define SCREEN_NEAR  0.1f

typedef struct {
    DirectX::XMFLOAT3    pos;           // vertex untransformed position
    DirectX::XMFLOAT2    tex;   // texture relative coordinates
} VIDEO_VERTEX;

typedef struct {
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
} VIDEO_MATRIX_BUFFER;

typedef struct {
    DirectX::XMFLOAT3    position;  //x, y, z
    DirectX::XMFLOAT4    color;     //r, g, b, a
} SIMPLE_VERTEX;

typedef struct {
    std::vector<char> data[3] = { std::vector<char>(0,0) };
    int linesize[3] = { 0 };
} VIDEO_FRAME;

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
    bool SetShaderResource();
    bool UpdateShaderResource(const VIDEO_FRAME& frame);
    bool SetSimpleVertexInputLayout();
    bool SetVideoVertexInputLayout();
    bool CreateMatrices();
    bool CreateMatrixBuffer();
    bool CreateSampleState();

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
    D3D11_VIEWPORT d3d_screen_viewport;
    D3D11_RASTERIZER_DESC raster_desc_;
    ID3D11RasterizerState* raster_state_ = NULL;
    UINT x4_msaa_uality_ = 0;
    ID3D11Buffer* vertex_buffer_ = NULL;
    DirectX::XMMATRIX world_matrix_;
    DirectX::XMMATRIX view_matrix_;
    DirectX::XMMATRIX projection_matrix_;

    ID3D11Texture2D* texture_planes_[3] = { NULL };
    ID3D11ShaderResourceView* texture_resource_views_[3] = { NULL };
    ID3D11Buffer* matrix_buffer_ = NULL;
    ID3D11SamplerState* sample_state_ = NULL;

    Graphic* graphic_ = nullptr;
    ColorCamera camera_;
    unsigned int width_ = 0;
    unsigned int height_ = 0;
};

