#pragma once

extern class GraphicEngine;

class Graphic
{
public:
    Graphic(GraphicEngine* engine);
    ~Graphic();

    virtual void Release();
    virtual bool SetInputLayout() { return true; };
    virtual void CreateInputBuffer() {};
    virtual void SetInputAssembler() {};
    virtual bool SetShaderParameters(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection) { return true; };
    virtual void Render() {};
    
protected:
    GraphicEngine* engine_ = NULL;
    ID3D11Buffer *vertex_buffer_ = NULL;
    ID3D11Buffer* index_buffer_ = NULL;
};

