#include "stdafx.h"
#include "Cube.h"
#include "utility.h"
#include <d3dcompiler.h>

using namespace DirectX;

Cube::Cube(GraphicEngine* engine)
    :Graphic::Graphic(engine), shader_(engine) {
}

Cube::~Cube() {
}

void Cube::Release() {
    __super::Release();
    shader_.Release();

    for (auto buffer : vertex_attribute_buffer_) {
        ReleaseCOMInterface(buffer);
    }
}

bool Cube::SetInputLayout() {
    return shader_.Init();
}

bool Cube::CreateInputBuffer() {
    if (!engine_) {
        return false;
    }

    XMFLOAT3 cube_vertice[] = {
        //down plane
        XMFLOAT3(-0.5f, -0.5f, -0.5f),
        XMFLOAT3(+0.5f, -0.5f, -0.5f),
        XMFLOAT3(+0.5f, -0.5f, +0.5f),
        XMFLOAT3(-0.5f, -0.5f, +0.5f),

        //up plane
        XMFLOAT3(-0.5f, +0.5f, -0.5f),
        XMFLOAT3(+0.5f, +0.5f, -0.5f),
        XMFLOAT3(+0.5f, +0.5f, +0.5f),
        XMFLOAT3(-0.5f, +0.5f, +0.5f)
    };

    XMFLOAT4 cube_color[] = {
        XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
        XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),

        XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
        XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
        XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)
    };

    WORD indices[cube::index_count] = { 
        0,1,2,
        0,2,3,
        4,5,6,
        4,6,7,
        0,4,7,
        0,7,3,
        1,5,6,
        1,6,2,
        3,2,6,
        3,6,7,
        0,1,5,
        0,5,4        
    };

    D3D11_BUFFER_DESC vertex_buffer_desc;
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = sizeof(XMFLOAT3) * cube::vertex_count;  //size of the buffer
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;            //type of the buffer
    vertex_buffer_desc.CPUAccessFlags = 0;
    vertex_buffer_desc.MiscFlags = 0;
    vertex_buffer_desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertex_data;
    vertex_data.pSysMem = cube_vertice;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;
    HRESULT hr = engine_->GetDevice()->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_attribute_buffer_[cube::Vertex_Index]);
    FailedDirect3DDebugString(hr, false, L"create vertex buffer in cube failed.";);

    vertex_buffer_desc.ByteWidth = sizeof(XMFLOAT4) * cube::vertex_count;
    vertex_data.pSysMem = cube_color;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;
    hr = engine_->GetDevice()->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_attribute_buffer_[cube::Color_Index]);
    FailedDirect3DDebugString(hr, false, L"create vertex buffer in cube failed.";);

    //description of the static index buffer.
    D3D11_BUFFER_DESC index_buffer_desc;
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = sizeof(WORD) * cube::index_count;  //size of the buffer
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;            //type of the buffer
    index_buffer_desc.CPUAccessFlags = 0;
    index_buffer_desc.MiscFlags = 0;
    index_buffer_desc.StructureByteStride = 0;
    //or we can use Map and UnMap
    D3D11_SUBRESOURCE_DATA index_data;
    index_data.pSysMem = indices;
    index_data.SysMemPitch = 0;
    index_data.SysMemSlicePitch = 0;
    hr = engine_->GetDevice()->CreateBuffer(&index_buffer_desc, &index_data, &index_buffer_);
    FailedDirect3DDebugString(hr, false, L"create index buffer in cube failed.";);

    ID3DBlob* vertex_shader_blob = NULL;
    ID3DBlob* pixcel_shader_blob = NULL;
    ID3DBlob* err_blob = NULL;
    hr = D3DCompileFromFile(L"../DXViewer/Cube.hlsl", NULL, NULL, "CubeVertexShader", "vs_5_0", D3DCOMPILE_DEBUG, 0, &vertex_shader_blob, &err_blob);
    if (FAILED(hr)) {
        char* msg = err_blob == NULL ? NULL : (char*)err_blob->GetBufferPointer();
        FailedDirect3DDebugString(hr, false, L"compile vertex shader failed.");
    }
    hr = D3DCompileFromFile(L"../DXViewer/Cube.hlsl", NULL, NULL, "CubePixelShader", "ps_5_0", D3DCOMPILE_DEBUG, 0, &pixcel_shader_blob, &err_blob);
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
    D3D11_INPUT_ELEMENT_DESC vertex_desc[2] = {
        { "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT ,cube::Vertex_Index, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT ,cube::Color_Index, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

void Cube::SetInputAssembler() {
    if (!engine_) {
        return;
    }

    unsigned int stride[cube::INDEX_END] = { sizeof(XMFLOAT3) , sizeof(XMFLOAT4)};
    unsigned int offset[cube::INDEX_END] = { 0 };

    //Set the vertex and index buffer to active in the "input assembler"(IA) so it can be rendered.
    //Once the GPU has an active vertex buffer it can then use the shader to render that buffer
    engine_->GetDeviceContext()->IASetVertexBuffers(0, cube::INDEX_END, vertex_attribute_buffer_, stride, offset);
    engine_->GetDeviceContext()->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R16_UINT, 0);
    engine_->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Cube::SetShaderParameters(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view,
    const DirectX::XMMATRIX& projection) {
    return shader_.SetShaderParameters(world, view, projection);
}

void Cube::Render() {
    shader_.Render(cube::index_count);
}