#pragma once
#include "GraphicEngine.h"

class DXViewerApp
{
public:
    DXViewerApp(HINSTANCE instance);
    ~DXViewerApp();

    bool Init();
    void Run();

private:
    static LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    HWND CreateViewerWindow(HINSTANCE instance, const unsigned int width, const unsigned int height);

private:
    HINSTANCE instance_ = NULL;
    HWND window_ = NULL;
    unsigned int width_ = 800;
    unsigned int height_ = 600;
    
    GraphicEngine engine_;
};

