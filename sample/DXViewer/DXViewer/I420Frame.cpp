#include "stdafx.h"
#include <fstream>
#include "I420Frame.h"
#include "utility.h"

using namespace DirectX;
namespace {
    const UINT vertex_count = 4;
    const UINT index_count = 6;
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

void I420Frame::CreateInputBuffer() {
    if (!engine_) {
        return;
    }

    VIDEO_VERTEX vertices[] = {
        //顶点坐标、顶点color，顶点坐标对应的纹理坐标
        { XMFLOAT3(-1.0f, 1.0f, 0.0f),XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 0.0f),XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 0.0f),XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 0.0f),XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }
    };

    WORD indices[index_count] = {
        0,1,2,
        0,2,3
    };

    //description of the static vertex buffer.
    D3D11_BUFFER_DESC vertex_buffer_desc;
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = sizeof(VIDEO_VERTEX) * vertex_count;  //size of the buffer
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;            //type of the buffer
    vertex_buffer_desc.CPUAccessFlags = 0;
    vertex_buffer_desc.MiscFlags = 0;
    vertex_buffer_desc.StructureByteStride = 0;
    //or we can use Map and UnMap
    D3D11_SUBRESOURCE_DATA vertex_data;
    vertex_data.pSysMem = vertices;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;
    HRESULT hr = engine_->GetDevice()->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer_);
    FailedDirect3DDebugString(hr, , L"create vertex buffer in frame failed.";);

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
    FailedDirect3DDebugString(hr, , L"create index buffer in frame failed.";);
}

void I420Frame::SetInputAssembler() {
    if (!engine_) {
        return;
    }

    unsigned int stride = sizeof(VIDEO_VERTEX);
    unsigned int offset = 0;

    //Set the vertex and index buffer to active in the "input assembler"(IA) so it can be rendered.
    //Once the GPU has an active vertex buffer it can then use the shader to render that buffer
    engine_->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);
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
        
        int count_of_y = frame.linesize[0] * frame_height;
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