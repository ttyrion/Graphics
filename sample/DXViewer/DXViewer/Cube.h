#pragma once
#include "Graphic.h"
#include "CubeShader.h"

namespace cube {
    enum INDEX {
        Vertex_Index,
        Color_Index,
        INDEX_END
    };

    const UINT vertex_count = 8;
    const UINT index_count = 36;
}

class Cube :
    public Graphic
{
public:
    Cube(GraphicEngine* engine);
    ~Cube();
    void Release() override;
    bool SetInputLayout() override;
    bool CreateInputBuffer() override;
    void SetInputAssembler() override;
    bool SetShaderParameters(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection) override;
    void Render() override;

private:
    CubeShader shader_;

    ID3D11Buffer* vertex_attribute_buffer_[cube::INDEX_END] = { NULL };
};

