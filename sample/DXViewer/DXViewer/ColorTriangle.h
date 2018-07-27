#pragma once
#include "Graphic.h"
#include <DirectXMath.h>
#include "ColorShader.h"

class ColorTriangle : public Graphic
{
public:
    ColorTriangle(GraphicEngine* engine);
    ~ColorTriangle();
    void Release() override;
    bool SetInputLayout() override;
    bool CreateInputBuffer() override;
    void SetInputAssembler() override;
    bool SetShaderParameters(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection) override;
    void Render() override;

private:
    ColorShader shader_;
};

