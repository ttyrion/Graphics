#include "stdafx.h"
#include "I420FrameShader.h"
#include <d3dcompiler.h>
#include "utility.h"

using namespace DirectX;

I420FrameShader::I420FrameShader(GraphicEngine* engine) {
    engine_ = engine;
}


I420FrameShader::~I420FrameShader() {
}

bool I420FrameShader::Init() {
    return CreateShaderAndInputLayout() && CreateMatrixBuffer() && CreateSamplerState();
}

void I420FrameShader::Release() {
    engine_ = NULL;
    ReleaseCOMInterface(matrix_buffer_);
    ReleaseCOMInterface(sampler_state_);
}

bool I420FrameShader::CreateShaderAndInputLayout() {
    if (!engine_) {
        return false;
    }
    ID3DBlob* vertex_shader_blob = NULL;
    ID3DBlob* pixcel_shader_blob = NULL;
    ID3DBlob* err_blob = NULL;
    HRESULT hr = D3DCompileFromFile(L"../DXViewer/I420Frame.hlsl", NULL, NULL, "I420FrameVertexShader", "vs_5_0", D3DCOMPILE_DEBUG, 0, &vertex_shader_blob, &err_blob);
    if (FAILED(hr)) {
        char* msg = err_blob == NULL ? NULL : (char*)err_blob->GetBufferPointer();
        FailedDirect3DDebugString(hr, false, L"compile vertex shader failed.");
    }
    hr = D3DCompileFromFile(L"../DXViewer/I420Frame.hlsl", NULL, NULL, "I420FramePixelShader", "ps_5_0", D3DCOMPILE_DEBUG, 0, &pixcel_shader_blob, &err_blob);
    if (FAILED(hr)) {
        char* msg = err_blob == NULL ? NULL : (char*)err_blob->GetBufferPointer();
        FailedDirect3DDebugString(hr, false, L"compile pixel shader failed.");
    }

    ID3D11VertexShader* vertex_shader;
    hr = engine_->GetDevice()->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), NULL, &vertex_shader);
    FailedDirect3DDebugString(hr, false, L"create vertex shader failed.");
    ID3D11PixelShader* pixel_shader;
    hr = engine_->GetDevice()->CreatePixelShader(pixcel_shader_blob->GetBufferPointer(), pixcel_shader_blob->GetBufferSize(), NULL, &pixel_shader);
    FailedDirect3DDebugString(hr, false, L"create pixel shader failed.");
    engine_->GetDeviceContext()->VSSetShader(vertex_shader, NULL, 0);
    engine_->GetDeviceContext()->PSSetShader(pixel_shader, NULL, 0);

    ID3D11InputLayout* input_layout = NULL;
    //This struct describes a single vertex property.
    /*
    Semantic	Values
    POSITION	float, float, float
    POSITIONT	float, float, float
    COLOR	    float, float, float, float
    PSIZE	    float
    */
    D3D11_INPUT_ELEMENT_DESC vertex_desc[3] = {
        { "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    int num = sizeof(vertex_desc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
    hr = engine_->GetDevice()->CreateInputLayout(vertex_desc, num, vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &input_layout);
    FailedDirect3DDebugString(hr, false, L"create input layout failed.");
    engine_->GetDeviceContext()->IASetInputLayout(input_layout);

    ReleaseCOMInterface(input_layout);
    ReleaseCOMInterface(vertex_shader);
    ReleaseCOMInterface(pixel_shader);
    ReleaseCOMInterface(vertex_shader_blob);
    ReleaseCOMInterface(pixcel_shader_blob);
    ReleaseCOMInterface(err_blob);

    return true;
}

bool I420FrameShader::CreateShaderResource(const int frame_width, const int frame_height) {
    //YUV三个分量 -> 三个纹理
    CD3D11_TEXTURE2D_DESC tex_desc(DXGI_FORMAT_R8_UNORM, frame_width, frame_height);
    tex_desc.MipLevels = 1;
    HRESULT hr = engine_->GetDevice()->CreateTexture2D(&tex_desc, NULL, &texture_planes_[0]);
    tex_desc.Width = frame_width / 2;
    tex_desc.Height = frame_height / 2;
    hr = engine_->GetDevice()->CreateTexture2D(&tex_desc, NULL, &texture_planes_[1]);
    hr = engine_->GetDevice()->CreateTexture2D(&tex_desc, NULL, &texture_planes_[2]);
    CD3D11_SHADER_RESOURCE_VIEW_DESC resource_view_desc(D3D11_SRV_DIMENSION_TEXTURE2D);
    for (int i = 0; i < 3; ++i) {
        //给每一个纹理资源都创建一个纹理视图，这样资源才能被GPU访问: 真正被绑定到渲染管线的是资源视图，而不是资源
        hr = engine_->GetDevice()->CreateShaderResourceView(texture_planes_[i], 
            &resource_view_desc, &texture_resource_views_[i]);
    }

    frame_width_ = frame_width;
    frame_height_ = frame_height;

    return true;
}

bool I420FrameShader::UpdateShaderResource(const VIDEO_FRAME& frame, const int frame_width, const int frame_height) {
    if (frame_width_ != frame_width || frame_height_ != frame_height) {
        for (int i = 0; i < 3; ++i) {
            ReleaseCOMInterface(texture_planes_[i]);
            ReleaseCOMInterface(texture_resource_views_[i]);
        }

        if (!CreateShaderResource(frame_width, frame_height)) {
            return false;
        }
    }

    for (int i = 0; i < 3; ++i) {
        if (!texture_planes_[i]) {
            break;
        }

        /*纹理资源更新方式：
        1.创建纹理时选择D3D11_USAGE_DEFAULT（GPU可读写）,然后只用UpdateSubresource更新纹理。
        2.创建纹理时选择D3D11_USAGE_DYNAMIC（GPU-R,CPU-W）加上D3D11_CPU_ACCESS_WRITE, 然后用 Map->memcpy->Unmap方式。
        第一种方式，即UpdateSubresource效率更高。
        */
        engine_->GetDeviceContext()->UpdateSubresource(texture_planes_[i], 0, NULL, frame.data[i].data(), frame.linesize[i], 0);
    }

    engine_->GetDeviceContext()->PSSetShaderResources(0, 3, texture_resource_views_);

    return true;
}

bool I420FrameShader::CreateMatrixBuffer() {
    D3D11_BUFFER_DESC matrix_buffer_desc;
    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrix_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    matrix_buffer_desc.ByteWidth = sizeof(XMMATRIX);           //size
    matrix_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;      //type
    matrix_buffer_desc.CPUAccessFlags = 0;
    matrix_buffer_desc.MiscFlags = 0;
    matrix_buffer_desc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    HRESULT hr = engine_->GetDevice()->CreateBuffer(&matrix_buffer_desc, NULL, &matrix_buffer_);
    FailedDirect3DDebugString(hr, false, L"create matrix buffer in cubeshader failed.");

    return true;
}

bool I420FrameShader::CreateSamplerState() {
    // Create the sample state
    D3D11_SAMPLER_DESC sampler_desc = {};
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr = engine_->GetDevice()->CreateSamplerState(&sampler_desc, &sampler_state_);
    
    return true;
}

bool I420FrameShader::SetShaderParameters(const XMMATRIX& w, const XMMATRIX &v, const XMMATRIX& p) {
    if (!matrix_buffer_) {
        Direct3DDebugString(L"Error in set shader parameters: matrix buffer is null.");

        return false;
    }

    XMMATRIX world = w;
    XMMATRIX view = v;
    XMMATRIX projection = p;
    //On the CPU, 2D arrays are generally stored in row-major ordering, 
    //But in HLSL, matrix declarations default to column-major ordering
    //world = XMMatrixTranspose(world);
    //view = XMMatrixTranspose(view);
    //projection = XMMatrixTranspose(projection);

    XMMATRIX matrix;
    //matrix.world = world;
    //matrix.view = view;
    //matrix.projection = projection;

    XMMATRIX mvp = w*v*p;
    matrix = XMMatrixTranspose(mvp);

    engine_->GetDeviceContext()->UpdateSubresource(matrix_buffer_, 0, NULL, &matrix, 0, 0);
    engine_->GetDeviceContext()->VSSetConstantBuffers(0, 1, &matrix_buffer_);

    return true;
}

void I420FrameShader::Render(const UINT index_count) {
    if (!engine_) {
        Direct3DDebugString(L"Error in cubeshader rendering: engine null.");
        return;
    }

    engine_->GetDeviceContext()->PSSetSamplers(0, 1, &sampler_state_);
    engine_->GetDeviceContext()->DrawIndexed(index_count,0,0);

    //engine_->GetDeviceContext()->Draw(8, 0);
}