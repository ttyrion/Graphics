#pragma once

typedef struct
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;

} MATRIX_BUFFER;

class ColorShader
{
public:
    ColorShader(GraphicEngine* engine);
    ~ColorShader();
    
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

