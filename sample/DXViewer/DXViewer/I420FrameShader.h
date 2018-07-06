#pragma once

class I420FrameShader
{
public:
    I420FrameShader(GraphicEngine* engine);
    ~I420FrameShader();

    bool Init();
    void Release();
    bool UpdateShaderResource(const VIDEO_FRAME& frame, const int frame_width, const int frame_height);
    bool SetShaderParameters(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
    void Render(const UINT index_count);

private:
    bool CreateShaderAndInputLayout();
    bool CreateMatrixBuffer();
    bool CreateSamplerState();
    bool CreateShaderResource(const int frame_width, const int frame_height);

    GraphicEngine* engine_ = NULL;
    ID3D11Buffer* matrix_buffer_ = NULL;
    ID3D11SamplerState* sampler_state_ = NULL;
    ID3D11Texture2D* texture_planes_[3] = { NULL };
    ID3D11ShaderResourceView* texture_resource_views_[3] = { NULL };
    UINT frame_width_ = 0;
    UINT frame_height_ = 0;
};

