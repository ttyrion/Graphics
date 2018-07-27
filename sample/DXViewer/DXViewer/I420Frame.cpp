#include "stdafx.h"
#include <fstream>
#include "I420Frame.h"
#include "utility.h"
#include <d3dcompiler.h>

using namespace DirectX;
namespace {
    const UINT vertex_count = 8;
    const UINT index_count = 12;
}

I420Frame::I420Frame(GraphicEngine* engine)
    :Graphic::Graphic(engine), shader_(engine) {
}

I420Frame::~I420Frame() {
}

void I420Frame::Release() {
    __super::Release();
    shader_.Release();
}

bool I420Frame::SetInputLayout() {
    return shader_.Init();
}

bool I420Frame::CreateInputBuffer() {
    if (!engine_) {
        return false;
    }

    //VIDEO_VERTEX vertices[] = {
    //    //顶点坐标、顶点color，顶点坐标对应的纹理坐标
    //    { XMFLOAT3(-2.0f, -2.0f, 2.0f),XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
    //    { XMFLOAT3(2.0f, -2.0f, 2.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
    //    { XMFLOAT3(2.0f, 2.0f, 2.0f),  XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
    //    { XMFLOAT3(-2.0f, 2.0f, 2.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

    //    { XMFLOAT3(-2.0f, -2.0f, 4.0f),XMFLOAT4( 0.0f,1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
    //    { XMFLOAT3(2.0f, -2.0f, 4.0f), XMFLOAT4( 0.0f, 1.0f,0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
    //    { XMFLOAT3(2.0f, 2.0f, 4.0f),  XMFLOAT4( 0.0f,1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
    //    { XMFLOAT3(-2.0f, 2.0f, 4.0f), XMFLOAT4( 0.0f,1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
    //};

    XMFLOAT3 vertex_pos[] = {
        XMFLOAT3(-2.0f, -2.0f, 2.0f),
        XMFLOAT3(2.0f, -2.0f, 2.0f),
        XMFLOAT3(2.0f, 2.0f, 2.0f),
        XMFLOAT3(-2.0f, 2.0f, 2.0f),

        XMFLOAT3(-2.0f, -2.0f, 4.0f),
        XMFLOAT3(2.0f, -2.0f, 4.0f),
        XMFLOAT3(2.0f, 2.0f, 4.0f),
        XMFLOAT3(-2.0f, 2.0f, 4.0f)
    };

    XMFLOAT4 vertex_color[] = {
        XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),

        XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) ,
        XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) ,
        XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) ,
        XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)
    };

    XMFLOAT2 texture[] = {
        XMFLOAT2(0.0f, 1.0f),
        XMFLOAT2(1.0f, 1.0f),
        XMFLOAT2(1.0f, 0.0f),
        XMFLOAT2(0.0f, 0.0f),

        XMFLOAT2(0.0f, 1.0f),
        XMFLOAT2(1.0f, 1.0f),
        XMFLOAT2(1.0f, 0.0f),
        XMFLOAT2(0.0f, 0.0f)
    };

    WORD indices[index_count] = {
        0,1,2,
        0,2,3, //near z
        4,5,6,
        4,6,7,
        //4,6,7 //far z
        //3,2,6,
        //3,6,7, //top
        //0,4,5,
        //0,5,1, //down
        //4,0,3,
        //4,3,7, //left
        //1,5,6,
        //1,6,2, //right
    };

    //description of the static vertex buffer.
    D3D11_BUFFER_DESC vertex_buffer_desc;
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = sizeof(XMFLOAT3) * vertex_count;  //size of the buffer
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;            //type of the buffer
    vertex_buffer_desc.CPUAccessFlags = 0;
    vertex_buffer_desc.MiscFlags = 0;
    vertex_buffer_desc.StructureByteStride = 0;
    //or we can use Map and UnMap
    D3D11_SUBRESOURCE_DATA vertex_data;
    vertex_data.pSysMem = vertex_pos;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;
    HRESULT hr = engine_->GetDevice()->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vbuffer_[0]);
    FailedDirect3DDebugString(hr, false, L"create vertex buffer in frame failed.";);

    vertex_buffer_desc.ByteWidth = sizeof(XMFLOAT4) * vertex_count;  //size of the buffer
    vertex_data.pSysMem = vertex_color;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;
    hr = engine_->GetDevice()->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vbuffer_[1]);
    FailedDirect3DDebugString(hr, false, L"create vertex buffer in frame failed.";);

    vertex_buffer_desc.ByteWidth = sizeof(XMFLOAT2) * vertex_count;  //size of the buffer
    vertex_data.pSysMem = texture;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;
    hr = engine_->GetDevice()->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vbuffer_[2]);
    FailedDirect3DDebugString(hr, false, L"create vertex buffer in frame failed.";);

    //description of the static index buffer.
    D3D11_BUFFER_DESC index_buffer_desc;
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = sizeof(WORD) * index_count;  //size of the buffer
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
    FailedDirect3DDebugString(hr, false, L"create index buffer in frame failed.");

    ID3DBlob* vertex_shader_blob = NULL;
    ID3DBlob* pixcel_shader_blob = NULL;
    ID3DBlob* err_blob = NULL;
    //运行时编译影响性能，实际的产品应用，应该先编译好着色器代码
    hr = D3DCompileFromFile(L"../DXViewer/I420Frame.hlsl", NULL, NULL, "I420FrameVertexShader", "vs_5_0", D3DCOMPILE_DEBUG, 0, &vertex_shader_blob, &err_blob);
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
        { "POSITIONTEST",1, DXGI_FORMAT_R32G32B32_FLOAT ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT ,1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT ,2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

void I420Frame::SetInputAssembler() {
    if (!engine_) {
        return;
    }

    unsigned int stride[3] = { sizeof(XMFLOAT3),sizeof(XMFLOAT4) ,sizeof(XMFLOAT2) };
    unsigned int offset[3] = {0};

    //Set the vertex and index buffer to active in the "input assembler"(IA) so it can be rendered.
    //Once the GPU has an active vertex buffer it can then use the shader to render that buffer
    engine_->GetDeviceContext()->IASetVertexBuffers(0, 3, vbuffer_, stride, offset);
    engine_->GetDeviceContext()->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R16_UINT,0);
    engine_->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool I420Frame::SetShaderParameters(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view,
    const DirectX::XMMATRIX& projection) {
    return shader_.SetShaderParameters(world, view, projection);
}

void I420Frame::Render() {
    std::ifstream ifs2;
    ifs2.open("frame.bin", std::ios::binary | std::ios::in);
    if (ifs2.is_open()) {
        VIDEO_FRAME frame;
        const UINT frame_width = 896;
        const UINT frame_height = 360;

        frame.linesize[0] = frame_width;
        frame.linesize[1] = 448;
        frame.linesize[2] = 448;
        
        int count_of_y = frame_width * frame_height;
        frame.data[0].resize(count_of_y, 0);
        ifs2.read(frame.data[0].data(), count_of_y);

        frame.data[1].resize(count_of_y / 4, 0);
        ifs2.read(frame.data[1].data(), count_of_y / 4);

        frame.data[2].resize(count_of_y / 4, 0);
        ifs2.read(frame.data[2].data(), count_of_y / 4);

        ifs2.close();

        shader_.UpdateShaderResource(frame, frame_width, frame_height);
    }

    shader_.Render(index_count);
}