#pragma once
#include <DirectXMath.h>

namespace direction {
    enum DIRECTION {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        FORWARD,
        BACKWARD,
    };
}

class FPSCamera
{
public:
    FPSCamera();
    ~FPSCamera();

    void SetPosition(const float x, const float y, const float z);
    DirectX::XMFLOAT3 GetPosition();

    void SetRight(DirectX::XMFLOAT3 right); //x-axis
    DirectX::XMFLOAT3 GetRight();

    void SetUp(DirectX::XMFLOAT3 up); //y
    DirectX::XMFLOAT3 GetUp();

    void SetLook(DirectX::XMFLOAT3 look); //z
    DirectX::XMFLOAT3 GetLook();

    void Walk(const direction::DIRECTION direction, const float dist);
    void Yaw(const float angle);
    void Pitch(const float angle);
    void Roll(const float angle);

    //更新相关矩阵
    void UpdateView();
    //获取相关矩阵
    DirectX::XMMATRIX GetViewMatrix() const { return XMLoadFloat4x4(&view_matrix_); }
    DirectX::XMMATRIX GetProjMatrix() const { return XMLoadFloat4x4(&proj_matrix_); }
    DirectX::XMMATRIX GetViewProjMatrix() const { return XMLoadFloat4x4(&view_matrix_) * XMLoadFloat4x4(&proj_matrix_); }

private:
    DirectX::XMFLOAT3 position_;
    DirectX::XMFLOAT3 right_;
    DirectX::XMFLOAT3 up_;
    DirectX::XMFLOAT3 look_;
    float aspect_ = 0.0f;
    float fovy_ = 0.0f;
    float nearz_ = 0.0f;
    float farz_ = 0.0f;

    DirectX::XMFLOAT4X4	view_matrix_;
    DirectX::XMFLOAT4X4	proj_matrix_;
};

