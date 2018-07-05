#pragma once
#include "Graphic.h"
#include "CubeShader.h"

class Cube :
    public Graphic
{
public:
    Cube(GraphicEngine* engine);
    ~Cube();
    void Release() override;
    bool SetInputLayout() override;
    void CreateInputBuffer() override;
    void SetInputAssembler() override;
    bool SetShaderParameters(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection) override;
    void Render() override;

private:
    CubeShader shader_;
};

