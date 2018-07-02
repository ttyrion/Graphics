#pragma once
#include <sstream>

#define Direct3DDebugString(msg) { std::wstringstream ss; ss << L"[Direct3D] "<< msg; ::OutputDebugString(ss.str().c_str()); }
#define FailedDirect3DDebugString(hr, r, msg) if(FAILED(hr)) { \
    std::wstringstream ss; ss << L"[Direct3D] "<< msg; \
    ::MessageBox(NULL, ss.str().c_str(), L"", MB_OK); \
    return r; \
}

#define ReleaseCOMInterface(x) { if(x){ x->Release(); x = 0; } }
