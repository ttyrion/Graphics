#include "stdafx.h"
#include "FPSCamera.h"

using namespace DirectX;

FPSCamera::FPSCamera() {
}

FPSCamera::~FPSCamera() {
}

void FPSCamera::SetPosition(const float x, const float y, const float z) {
    position_ = XMFLOAT3(x, y, z);
}

XMFLOAT3 FPSCamera::GetPosition() {
    return position_;
}

void FPSCamera::SetRight(XMFLOAT3 right) {//x-axis
    right_ = right;
}

DirectX::XMFLOAT3 FPSCamera::GetRight() {
    return right_;
}

void FPSCamera::SetUp(XMFLOAT3 up) { //y
    up_ = up;
}

DirectX::XMFLOAT3 FPSCamera::GetUp() {
    return up_;
}

void FPSCamera::SetLook(XMFLOAT3 look) { //z
    look_ = look;
}

DirectX::XMFLOAT3 FPSCamera::GetLook() {
    return look_;
}

void FPSCamera::Walk(const direction::DIRECTION direction, const float dist) {
    switch (direction) {
    case direction::FORWARD: {
        position_.z += dist;
    }
    break;
    case direction::BACKWARD: {
        position_.z -= dist;
    }
    break;
    case direction::LEFT: {
        position_.x -= dist;
    }
    break;
    case direction::RIGHT: {
        position_.x += dist;
    }
    break;
    case direction::UP: {
        position_.y += dist;
    }
    break;
    case direction::DOWN: {
        position_.y -= dist;
    }
    break;
    default:
        break;
    }
}

void FPSCamera::Yaw(const float angle) {
    XMMATRIX rotation = XMMatrixRotationY(angle);

    //DirectXMath uses a completely wrong and misleading name.
    //XMVector3TransformNormal transforms 3D directions (w-coordinate is implicitly set to 0).
    XMStoreFloat3(&right_, XMVector3TransformNormal(XMLoadFloat3(&right_), rotation));
    XMStoreFloat3(&up_, XMVector3TransformNormal(XMLoadFloat3(&up_), rotation));
    XMStoreFloat3(&look_, XMVector3TransformNormal(XMLoadFloat3(&look_), rotation));
}


void FPSCamera::Pitch(const float angle) {
    //XMMatrixRotationX
    XMMATRIX rotation = XMMatrixRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), angle);
    XMStoreFloat3(&up_, XMVector3TransformNormal(XMLoadFloat3(&up_), rotation));
    XMStoreFloat3(&look_, XMVector3TransformNormal(XMLoadFloat3(&look_), rotation));
}

void FPSCamera::Roll(const float angle) {
    XMMATRIX rotation = XMMatrixRotationZ(angle);
    XMStoreFloat3(&up_, XMVector3TransformNormal(XMLoadFloat3(&up_), rotation));
    XMStoreFloat3(&look_, XMVector3TransformNormal(XMLoadFloat3(&look_), rotation));
}

void FPSCamera::UpdateView() {
    XMVECTOR right = XMLoadFloat3(&right_);
    XMVECTOR up = XMLoadFloat3(&up_);
    XMVECTOR look = XMLoadFloat3(&look_);
    XMVECTOR position = XMLoadFloat3(&position_);

    //对三个向量进行正交化以及归一化:因为计算过程会有些误差
    right = XMVector3Normalize(XMVector3Cross(up, look));
    up = XMVector3Normalize(XMVector3Cross(look, right));
    look = XMVector3Normalize(look);

    float p_dot_x = -XMVectorGetX(XMVector3Dot(position, right));
    float p_dot_y = -XMVectorGetX(XMVector3Dot(position, up));
    float p_dot_z = -XMVectorGetX(XMVector3Dot(position, look));

    XMStoreFloat3(&right_, right);
    XMStoreFloat3(&up_, up);
    XMStoreFloat3(&look_, look);
    XMStoreFloat3(&position_, position);

    view_matrix_(0, 0) = right_.x;	view_matrix_(0, 1) = up_.x;	view_matrix_(0, 2) = look_.x;	view_matrix_(0, 3) = 0;
    view_matrix_(1, 0) = right_.y;	view_matrix_(1, 1) = up_.y;	view_matrix_(1, 2) = look_.y;	view_matrix_(1, 3) = 0;
    view_matrix_(2, 0) = right_.z;	view_matrix_(2, 1) = up_.z;	view_matrix_(2, 2) = look_.z;	view_matrix_(2, 3) = 0;
    view_matrix_(3, 0) = p_dot_x;
    view_matrix_(3, 1) = p_dot_y;
    view_matrix_(3, 2) = p_dot_z;
    view_matrix_(3, 3) = 1;
}