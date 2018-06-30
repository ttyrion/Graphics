#include "stdafx.h"
#include "GraphicEngine.h"
#include "utility.h"
#include <assert.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include <vector>

using namespace DirectX;

GraphicEngine::GraphicEngine() {

}

GraphicEngine::~GraphicEngine() {

}

bool GraphicEngine::InitDirect3D(HWND render_window, const unsigned int width, const unsigned int height) {
    UINT create_device_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    //激活调试层。当指定调试标志值后，Direct3D会向VC++的输出窗口发送调试信息
    create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL feature_level;
    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = render_window;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, create_device_flags, 0, 0, D3D11_SDK_VERSION, &sd, &d3d_swap_chain_, &d3d_device_, &feature_level, &d3d_immediate_context_);
    if (FAILED(hr)) {
        Direct3DDebugString(L"D3D11CreateDeviceAndSwapChain Failed.");
        return false;
    }

    if (feature_level != D3D_FEATURE_LEVEL_11_0 ) {
        Direct3DDebugString(L"Feature Level 11 unsupported.");
        return false;
    }

    hr = d3d_device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &x4_msaa_uality_);
    if (FAILED(hr)) {
        Direct3DDebugString(L"CheckMultisampleQualityLevels Failed.");
        return false;
    }
    assert(x4_msaa_uality_ > 0);

    ID3DBlob* vertex_shader_blob = NULL;
    ID3DBlob* pixcel_shader_blob = NULL;
    ID3DBlob* err_blob = NULL;
    hr = D3DCompileFromFile(L"../DXViewer/shaders.hlsl", NULL, NULL, "DXViewerVertexShader", "vs_5_0", D3DCOMPILE_DEBUG, 0, &vertex_shader_blob, &err_blob);
    if (FAILED(hr)) {
        char* msg = err_blob == NULL ? NULL : (char*)err_blob->GetBufferPointer();
        return false;
    }
    hr = D3DCompileFromFile(L"../DXViewer/shaders.hlsl", NULL, NULL, "DXViewerPixelShader", "ps_5_0", D3DCOMPILE_DEBUG, 0, &pixcel_shader_blob, &err_blob);
    if (FAILED(hr)) {
        char* msg = err_blob == NULL ? NULL : (char*)err_blob->GetBufferPointer();
        return false;
    }

    ID3D11VertexShader* vertex_shader;
    d3d_device_->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), NULL, &vertex_shader);
    ID3D11PixelShader* pixel_shader;
    d3d_device_->CreatePixelShader(pixcel_shader_blob->GetBufferPointer(), pixcel_shader_blob->GetBufferSize(), NULL, &pixel_shader);
    ID3D11InputLayout* input_layout = NULL;

    D3D11_INPUT_ELEMENT_DESC video_vertex_desc[4] = {
        { "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT ,0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",0,DXGI_FORMAT_R32_FLOAT,0,12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT ,0, 13, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0, 29, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    d3d_device_->CreateInputLayout(video_vertex_desc, 4, vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &input_layout);


    vertex_shader_blob->Release();
    pixcel_shader_blob->Release();
    err_blob == NULL ? NULL : err_blob->Release();

    width_ = width;
    height_ = height;

    return true;
}

void GraphicEngine::ReSizeRender(const unsigned int width, const unsigned int height) {
    assert(d3d_device_);
    assert(d3d_immediate_context_);
    assert(d3d_swap_chain_);

    ReleaseCOMInterface(d3d_depth_stencil_buffer_);
    ReleaseCOMInterface(d3d_render_target_view_);
    ReleaseCOMInterface(d3d_depth_stencil_view_);

    HRESULT hr = d3d_swap_chain_->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (FAILED(hr)) {
        Direct3DDebugString(L"ResizeBuffers Failed.");
        return;
    }

    ID3D11Texture2D* backBuffer;
    hr = d3d_swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    if (FAILED(hr)) {
        Direct3DDebugString(L"GetBuffer Failed.");
        return;
    }

    hr = d3d_device_->CreateRenderTargetView(backBuffer, 0, &d3d_render_target_view_);
    if (FAILED(hr)) {
        Direct3DDebugString(L"CreateRenderTargetView Failed.");
        return;
    }
    ReleaseCOMInterface(backBuffer);

    D3D11_TEXTURE2D_DESC depth_stencil_desc;
    depth_stencil_desc.Width = width;
    depth_stencil_desc.Height = height;
    depth_stencil_desc.MipLevels = 1;
    depth_stencil_desc.ArraySize = 1;
    depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_desc.SampleDesc.Count = 1;
    depth_stencil_desc.SampleDesc.Quality = 0;
    depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_stencil_desc.CPUAccessFlags = 0;
    depth_stencil_desc.MiscFlags = 0;

    hr = d3d_device_->CreateTexture2D(&depth_stencil_desc, 0, &d3d_depth_stencil_buffer_);
    if (FAILED(hr)) {
        Direct3DDebugString(L"create depth stencil buffer failed.");
        return;
    }

    hr = d3d_device_->CreateDepthStencilView(d3d_depth_stencil_buffer_, 0, &d3d_depth_stencil_view_);
    if (FAILED(hr)) {
        Direct3DDebugString(L"create depth stencil view failed.");
        return;
    }

    // Bind the render target view and depth/stencil view to the pipeline.
    d3d_immediate_context_->OMSetRenderTargets(1, &d3d_render_target_view_, d3d_depth_stencil_view_);

    d3d_screen_viewport.TopLeftX = 0;
    d3d_screen_viewport.TopLeftY = 0;
    d3d_screen_viewport.Width = static_cast<float>(width);
    d3d_screen_viewport.Height = static_cast<float>(height);
    d3d_screen_viewport.MinDepth = 0.0f;
    d3d_screen_viewport.MaxDepth = 1.0f;
    d3d_immediate_context_->RSSetViewports(1, &d3d_screen_viewport);

    width_ = width;
    height_ = height;
}

void GraphicEngine::UpdateScene() {
    UpdateTestScene();
}

void GraphicEngine::DrawScene() {
    assert(d3d_immediate_context_);
    assert(d3d_swap_chain_);

    d3d_swap_chain_->Present(0, 0);
}

void GraphicEngine::UpdateTestScene() {
    assert(d3d_immediate_context_);
    assert(d3d_render_target_view_);
    assert(d3d_swap_chain_);

    XMVECTORF32 blue = { 0.0f,0.0f,1.0f,1.0f };
    d3d_immediate_context_->ClearRenderTargetView(d3d_render_target_view_, reinterpret_cast<const float*>(&blue));

    ID3D11Texture2D* backBuffer;
    HRESULT hr = d3d_swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    if (FAILED(hr)) {
        Direct3DDebugString(L"GetBuffer failed when update scene.");
        return;
    }

    std::ifstream ifs;
    ifs.open("frame.bin", std::ios::binary | std::ios::in);
    if (ifs.is_open()) {
        ifs.seekg(0, std::ios_base::end);
        int size = (int)ifs.tellg();
        ifs.seekg(0, std::ios_base::beg);
        std::vector<char> frame_buffer(size, 0);
        ifs.read(&frame_buffer[0], size);
        ifs.close();

        ID3D11Texture2D* temp;
        D3D11_TEXTURE2D_DESC video_texture_desc;
        video_texture_desc.Width = width_;
        video_texture_desc.Height = height_;
        video_texture_desc.MipLevels = 1;
        video_texture_desc.ArraySize = 1;
        video_texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        video_texture_desc.SampleDesc.Count = 1;
        video_texture_desc.SampleDesc.Quality = 0;
        video_texture_desc.Usage = D3D11_USAGE_DYNAMIC;
        video_texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        video_texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        video_texture_desc.MiscFlags = 0;
        hr = d3d_device_->CreateTexture2D(&video_texture_desc, 0, &temp);
        if (FAILED(hr)) {
            Direct3DDebugString(L"create temp texture failed when update scene.");
            return;
        }

        D3D11_MAPPED_SUBRESOURCE resource;
        hr = d3d_immediate_context_->Map(temp, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
        if (FAILED(hr)) {
            Direct3DDebugString(L"Map failed when update scene.");
            return;
        }

        const int frame_width = 896, frame_height = 360;
        std::vector<char> buffer_with_alpha(4 * frame_width * frame_height, 0);
        int county = frame_buffer.size() * 2 / 3;
        int iu = county;
        int iv = county + county / 4;

        auto clip = [](int value) {
            if (value <= 0) {
                return 0;
            }
            if (value >= 255) {
                return 255;
            }
            return value;
        };

        for (int i = 0; i < county; ++i) {
            int C = ((int)(frame_buffer[i] & 0xff) - 16);
            int D = ((int)(frame_buffer[iu + frame_width / 2 * (i / frame_width / 2) + i % frame_width / 2] & 0xff) - 128);
            int E = ((int)(frame_buffer[iv + frame_width / 2 * (i / frame_width / 2) + i % frame_width / 2] & 0xff) - 128);
            //R
            buffer_with_alpha[i * 4] = clip((298 * C + 409 * E + 128) >> 8);
            //G
            buffer_with_alpha[i * 4 + 1] = clip((298 * C - 100 * D - 208 * E + 128) >> 8);
            //B
            buffer_with_alpha[i * 4 + 2] = clip((298 * C + 516 * D + 128) >> 8);
            buffer_with_alpha[i * 4 + 3] = 255;
        }

        int frame_line_size = frame_width * 4;
        char* dest = (char*)resource.pData;
        int index = 0;
        for (unsigned long i = 0; i < frame_height; i++) {
            memcpy(dest, &buffer_with_alpha[index], frame_line_size);
            dest += resource.RowPitch;
            index += frame_line_size;
        }

        d3d_immediate_context_->Unmap(temp, 0);
        d3d_immediate_context_->CopyResource(backBuffer, temp);
        ReleaseCOMInterface(temp);
    }

    ReleaseCOMInterface(backBuffer);
}