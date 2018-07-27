#pragma once
#include "Graphic.h"
#include "I420FrameShader.h"

class I420Frame :
    public Graphic
{
public:
    I420Frame(GraphicEngine* engine);
    ~I420Frame();
    void Release() override;
    bool SetInputLayout() override;
    void CreateInputBuffer() override;
    void SetInputAssembler() override;
    bool SetShaderParameters(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection) override;
    void Render() override;

private:
    I420FrameShader shader_;
    ID3D11Buffer* vbuffer_[3] = { NULL };
};