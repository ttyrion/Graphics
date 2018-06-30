#pragma once
#include <sstream>

#define Direct3DDebugString(msg) { std::wstringstream ss; ss << L"[Direct3D] "<< msg; ::OutputDebugString(ss.str().c_str()); }
#define ReleaseCOMInterface(x) { if(x){ x->Release(); x = 0; } }
