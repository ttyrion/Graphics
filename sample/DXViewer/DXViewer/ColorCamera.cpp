#include "stdafx.h"
#include "ColorCamera.h"
using namespace DirectX;

ColorCamera::ColorCamera()
{
}


ColorCamera::~ColorCamera()
{
}

DirectX::XMVECTOR& ColorCamera::GetPosition() {
    return position_;
}

DirectX::XMFLOAT3& ColorCamera::GetRotation() {
    return rotation_;
}

DirectX::XMMATRIX ColorCamera::GetViewMatrix() {
    //the vector that points upwards, it's 3D vector
    XMVECTOR up_vector = { 0.0f, 1.0f, 0.0f, 1.0f};
    // where the camera is looking by default
    XMVECTOR look_at = { 0.0f, 0.0f, 1.0f,0.0f };
    //yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
    float yaw = rotation_.x * 0.0174532925f;
    float pitch = rotation_.y * 0.0174532925f;
    float roll = rotation_.z * 0.0174532925f;
    
    XMMATRIX rotate_matrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    up_vector = XMVector3TransformCoord(up_vector, rotate_matrix);
    look_at = XMVector3TransformCoord(look_at, rotate_matrix);
    look_at = look_at + position_;

    //Finally create the view matrix from the three updated vectors
    XMMATRIX view_matrix = XMMatrixLookAtLH(position_, look_at, up_vector);

    return view_matrix;
}