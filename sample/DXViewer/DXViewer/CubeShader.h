#pragma once

class CubeShader
{
public:
    CubeShader(GraphicEngine* engine);
    ~CubeShader();

    bool Init();
    void Release();
    bool SetShaderParameters(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
    void Render(const UINT index_count);

private:
    bool CreateShaderAndInputLayout();
    bool CreateMatrixBuffer();

    GraphicEngine* engine_ = NULL;
    ID3D11Buffer* matrix_buffer_ = NULL;
};

