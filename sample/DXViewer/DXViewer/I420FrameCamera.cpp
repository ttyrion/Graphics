#include "stdafx.h"
#include "I420FrameCamera.h"
using namespace DirectX;

I420FrameCamera::I420FrameCamera()
{
}


I420FrameCamera::~I420FrameCamera()
{
}

DirectX::XMVECTOR& I420FrameCamera::GetPosition() {
    return position_;
}

DirectX::XMFLOAT3& I420FrameCamera::GetRotation() {
    return rotation_;
}

DirectX::XMMATRIX I420FrameCamera::GetViewMatrix() {
    return XMMatrixIdentity();
    // Initialize the view matrix
    XMVECTOR eye = XMVectorSet(0.5f, 0.5f, 0.0f, 0.0f);
    XMVECTOR look_at = XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f);
    XMVECTOR up_vector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    return XMMatrixLookAtLH(eye, look_at, up_vector);
}