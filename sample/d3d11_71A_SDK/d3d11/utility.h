#pragma once
#include <sstream>

#define Direct3DDebugString(msg) { std::wstringstream ss; ss << L"[Direct3D] "<< msg; ::OutputDebugString(ss.str().c_str()); }
#define FailedDirect3DDebugString(hr, r, msg) if(FAILED(hr)) { \
    std::wstringstream ss; ss << L"[Direct3D] "<< msg; \
    ::MessageBox(NULL, ss.str().c_str(), L"", MB_OK); \
    return r; \
}

#define ReleaseCOMInterface(x) { if(x){ x->Release(); x = 0; } }

typedef struct {
    DirectX::XMFLOAT3    pos;           // vertex untransformed position
    DirectX::XMFLOAT4    color;
    DirectX::XMFLOAT2    tex;   // texture relative coordinates
} VIDEO_VERTEX;

typedef struct {
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
} VIDEO_MATRIX_BUFFER;

typedef struct {
    DirectX::XMFLOAT3    position;  //x, y, z
    DirectX::XMFLOAT4    color;     //r, g, b, a
} SIMPLE_VERTEX;

typedef struct {
    std::vector<char> data[3] = { std::vector<char>(0,0) };
    int linesize[3] = { 0 };
} VIDEO_FRAME;

typedef struct
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
} ColorVertex;

typedef struct
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;

} MATRIX_BUFFER;


std::string LoadShader(const std::string& cso);