#include "stdafx.h"
#include "CubeCamera.h"
using namespace DirectX;

CubeCamera::CubeCamera()
{
}


CubeCamera::~CubeCamera()
{
}

DirectX::XMVECTOR& CubeCamera::GetPosition() {
    return position_;
}

DirectX::XMFLOAT3& CubeCamera::GetRotation() {
    return rotation_;
}

DirectX::XMMATRIX CubeCamera::GetViewMatrix() {
    // Initialize the view matrix
    XMVECTOR eye = XMVectorSet(-0.5f, -0.5f, -0.5f, 0.0f);
    XMVECTOR look_at = XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f);
    XMVECTOR up_vector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    return XMMatrixLookAtLH(eye, look_at, up_vector);
}