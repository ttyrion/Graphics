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
    //bool CreateShaderAndInputLayout();
    bool CreateMatrixBuffer();
    bool CreateSamplerState();
    bool CreateShaderResource(const int frame_width, const int frame_height);

    GraphicEngine* engine_ = NULL;
    ID3D11Buffer* matrix_buffer_ = NULL;
    //控制着色器
    ID3D11SamplerState* sampler_state_ = NULL;
    //用三层文理资源分别访问一帧视频的y,u,v数据，相应地，着色器中也要定义三个纹理资源
    ID3D11Texture2D* texture_planes_[3] = { NULL };
    ID3D11ShaderResourceView* texture_resource_views_[3] = { NULL };
    UINT frame_width_ = 0;
    UINT frame_height_ = 0;
};

