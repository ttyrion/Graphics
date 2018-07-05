#pragma once
#include "Graphic.h"
#include <DirectXMath.h>
#include "ColorShader.h"

typedef struct
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
} ColorVertex;

class ColorTriangle : public Graphic
{
public:
    ColorTriangle(GraphicEngine* engine);
    ~ColorTriangle();
    void Release() override;
    bool SetInputLayout() override;
    void CreateInputBuffer() override;
    void SetInputAssembler() override;
    bool SetShaderParameters(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection) override;
    void Render() override;

private:
    ColorShader shader_;
};

