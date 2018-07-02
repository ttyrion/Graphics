#include "stdafx.h"
#include "GraphicEngine.h"
#include "utility.h"
#include <assert.h>
#include <d3dcompiler.h>
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


    ZeroMemory(&depth_stenci_desc_, sizeof(depth_stenci_desc_));
    // Set up the description of the stencil state.
    depth_stenci_desc_.DepthEnable = true;
    depth_stenci_desc_.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stenci_desc_.DepthFunc = D3D11_COMPARISON_LESS;
    depth_stenci_desc_.StencilEnable = true;
    depth_stenci_desc_.StencilReadMask = 0xFF;
    depth_stenci_desc_.StencilWriteMask = 0xFF;
    // Stencil operations if pixel is front-facing.
    depth_stenci_desc_.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stenci_desc_.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depth_stenci_desc_.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stenci_desc_.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // Stencil operations if pixel is back-facing.
    depth_stenci_desc_.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stenci_desc_.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depth_stenci_desc_.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stenci_desc_.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    hr = d3d_device_->CreateDepthStencilState(&depth_stenci_desc_, &depth_stenci_state_);
    FailedDirect3DDebugString(hr, false, L"create depth stencil state failed.");

    ZeroMemory(&disable_depth_stenci_desc_, sizeof(disable_depth_stenci_desc_));
    // Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
    // that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
    disable_depth_stenci_desc_.DepthEnable = false;
    disable_depth_stenci_desc_.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    disable_depth_stenci_desc_.DepthFunc = D3D11_COMPARISON_LESS;
    disable_depth_stenci_desc_.StencilEnable = true;
    disable_depth_stenci_desc_.StencilReadMask = 0xFF;
    disable_depth_stenci_desc_.StencilWriteMask = 0xFF;
    disable_depth_stenci_desc_.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    disable_depth_stenci_desc_.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    disable_depth_stenci_desc_.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    disable_depth_stenci_desc_.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    disable_depth_stenci_desc_.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    disable_depth_stenci_desc_.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    disable_depth_stenci_desc_.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    disable_depth_stenci_desc_.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    hr = d3d_device_->CreateDepthStencilState(&disable_depth_stenci_desc_, &disable_depth_stenci_state_);
    FailedDirect3DDebugString(hr, false, L"create disable depth stencil state failed.");

    //diable the depth buffer
    d3d_immediate_context_->OMSetDepthStencilState(disable_depth_stenci_state_, 1);

    //Setup the raster description which will determine how and what polygons will be drawn.
    raster_desc_.AntialiasedLineEnable = false;
    raster_desc_.CullMode = D3D11_CULL_BACK;
    raster_desc_.DepthBias = 0;
    raster_desc_.DepthBiasClamp = 0.0f;
    raster_desc_.DepthClipEnable = true;
    raster_desc_.FillMode = D3D11_FILL_SOLID;
    raster_desc_.FrontCounterClockwise = false;
    raster_desc_.MultisampleEnable = false;
    raster_desc_.ScissorEnable = false;
    raster_desc_.SlopeScaledDepthBias = 0.0f;
    // Create the rasterizer state from the description we just filled out.
    hr = d3d_device_->CreateRasterizerState(&raster_desc_, &raster_state_);
    FailedDirect3DDebugString(hr, false, L"create raster state failed.");
    //Now set the rasterizer state.
    d3d_immediate_context_->RSSetState(raster_state_);

    //world_matrix_ = XMMatrixIdentity();
    ////an orthographic projection matrix is needed rendering in 2D in place of the regular 3D projection matrix
    //orthographic_matrix_ = XMMatrixOrthographicLH((float)width, (float)height, SCREEN_NEAR, SCREEN_DEPTH);

    ID3DBlob* vertex_shader_blob = NULL;
    ID3DBlob* pixcel_shader_blob = NULL;
    ID3DBlob* err_blob = NULL;
    hr = D3DCompileFromFile(L"../DXViewer/shaders.hlsl", NULL, NULL, "DXViewerVertexShader", "vs_5_0", D3DCOMPILE_DEBUG, 0, &vertex_shader_blob, &err_blob);
    if (FAILED(hr)) {
        char* msg = err_blob == NULL ? NULL : (char*)err_blob->GetBufferPointer();
        FailedDirect3DDebugString(hr, false, L"compile vertex shader failed.");
    }
    hr = D3DCompileFromFile(L"../DXViewer/shaders.hlsl", NULL, NULL, "DXViewerPixelShader", "ps_5_0", D3DCOMPILE_DEBUG, 0, &pixcel_shader_blob, &err_blob);
    if (FAILED(hr)) {
        char* msg = err_blob == NULL ? NULL : (char*)err_blob->GetBufferPointer();
        FailedDirect3DDebugString(hr, false, L"compile pixel shader failed.");
    }

    ID3D11InputLayout* input_layout = NULL;
    //This struct describes a single vertex property.
    /*
        Semantic	Values
        POSITION	float, float, float
        POSITIONT	float, float, float
        COLOR	    float, float, float, float
        PSIZE	    float
    */
    D3D11_INPUT_ELEMENT_DESC simple_video_vertex_desc[2] = {
        { "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    ID3D11VertexShader* vertex_shader;
    hr = d3d_device_->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), NULL, &vertex_shader);
    FailedDirect3DDebugString(hr, false, L"create vertex shader failed.");
    ID3D11PixelShader* pixel_shader;
    hr = d3d_device_->CreatePixelShader(pixcel_shader_blob->GetBufferPointer(), pixcel_shader_blob->GetBufferSize(), NULL, &pixel_shader);
    FailedDirect3DDebugString(hr, false, L"create pixel shader failed.");
    
    d3d_immediate_context_->VSSetShader(vertex_shader, NULL, 0);
    d3d_immediate_context_->PSSetShader(pixel_shader,NULL, 0);

    hr = d3d_device_->CreateInputLayout(simple_video_vertex_desc, 2, vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &input_layout);
    FailedDirect3DDebugString(hr, false, L"create input layout failed.");
    d3d_immediate_context_->IASetInputLayout(input_layout);

    ReleaseCOMInterface(vertex_shader_blob);
    ReleaseCOMInterface(pixcel_shader_blob);
    ReleaseCOMInterface(err_blob);
    ReleaseCOMInterface(input_layout);
    ReleaseCOMInterface(vertex_shader);
    ReleaseCOMInterface(pixel_shader);

    //创建顶点缓冲
    SIMPLE_VERTEX simple_vertices[] = {
        { DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
    };

    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    //D3D11_USAGE_DYNAMIC: CPU Write Only; GPU Read Only
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(VIDEO_VERTEX) * 3;
    //BindFlags tells Direct3D what kind of buffer to make
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //no CPU access necessary;
    vbd.MiscFlags = 0;

    //D3D11_SUBRESOURCE_DATA init_data;
    //ZeroMemory(&init_data, sizeof(init_data));
    //init_data.pSysMem = vertices; //Memory in CPU to copy in to GPU
    //hr = d3d_device_->CreateBuffer(&vbd, &init_data, &vertex_buffer_);

    hr = d3d_device_->CreateBuffer(&vbd, NULL, &vertex_buffer_);
    FailedDirect3DDebugString(hr, false, L"create vertex buffer failed.");
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    //like old "lock" in d3d9
    //D3D11_MAP_WRITE_DISCARD: Previous contents of buffer are erased, and new buffer is opened for writing.
    d3d_immediate_context_->Map(vertex_buffer_, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapped_resource);
    FailedDirect3DDebugString(hr, false, L"map vertex buffer failed.");
    memcpy(mapped_resource.pData, simple_vertices, sizeof(simple_vertices));
    d3d_immediate_context_->Unmap(vertex_buffer_, NULL);

    

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

    //switch the back buffer and the front buffer
    d3d_swap_chain_->Present(0, 0);
}

void GraphicEngine::UpdateTestScene() {
    assert(d3d_immediate_context_);
    assert(d3d_render_target_view_);
    assert(d3d_swap_chain_);

    XMVECTORF32 blue = { 0.0f,0.0f,1.0f,1.0f };
    //d3d_immediate_context_->ClearRenderTargetView(d3d_render_target_view_, reinterpret_cast<const float*>(&blue));

    UINT offset = 0;
    UINT stride = sizeof(SIMPLE_VERTEX);
    d3d_immediate_context_->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);
    d3d_immediate_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //draw the vertex buffer to the back buffer
    d3d_immediate_context_->Draw(3, 0);

    return;

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