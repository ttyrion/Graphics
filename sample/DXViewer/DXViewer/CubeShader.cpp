#include "stdafx.h"
#include "CubeShader.h"
#include <d3dcompiler.h>
#include "utility.h"

using namespace DirectX;

CubeShader::CubeShader(GraphicEngine* engine) {
    engine_ = engine;
}


CubeShader::~CubeShader() {
}

bool CubeShader::Init() {
    return CreateMatrixBuffer();
}

void CubeShader::Release() {
    engine_ = NULL;
    ReleaseCOMInterface(matrix_buffer_);
}

bool CubeShader::CreateShaderAndInputLayout() {
    if (!engine_) {
        return false;
    }
    

    return true;
}

bool CubeShader::CreateMatrixBuffer() {
    D3D11_BUFFER_DESC matrix_buffer_desc;
    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrix_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    matrix_buffer_desc.ByteWidth = sizeof(MATRIX_BUFFER);           //size
    matrix_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;      //type
    matrix_buffer_desc.CPUAccessFlags = 0;
    matrix_buffer_desc.MiscFlags = 0;
    matrix_buffer_desc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    HRESULT hr = engine_->GetDevice()->CreateBuffer(&matrix_buffer_desc, NULL, &matrix_buffer_);
    FailedDirect3DDebugString(hr, false, L"create matrix buffer in cubeshader failed.");

    return true;
}

bool CubeShader::SetShaderParameters(const XMMATRIX& w, const XMMATRIX &v, const XMMATRIX& p) {
    if (!matrix_buffer_) {
        Direct3DDebugString(L"Error in set shader parameters: matrix buffer is null.");

        return false;
    }
    
    XMMATRIX world = w;
    XMMATRIX view = v;
    XMMATRIX projection = p;
    world = XMMatrixTranspose(world);
    view = XMMatrixTranspose(view);
    projection = XMMatrixTranspose(projection);

    MATRIX_BUFFER matrix;
    matrix.world = world;
    matrix.view = view;
    matrix.projection = projection;
    engine_->GetDeviceContext()->UpdateSubresource(matrix_buffer_, 0, NULL, &matrix, 0, 0);
    engine_->GetDeviceContext()->VSSetConstantBuffers(0, 1, &matrix_buffer_);

    return true;
}

void CubeShader::Render(const UINT index_count) {
    if (!engine_) {
        Direct3DDebugString(L"Error in cubeshader rendering: engine null.");
        return;
    }

    //engine_->GetDeviceContext()->DrawIndexed(index_count, 0, 0);
    engine_->GetDeviceContext()->DrawIndexedInstanced(index_count, 3, 0, 0, 0);
}