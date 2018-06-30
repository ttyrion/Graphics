#pragma once
#include "GraphicEngine.h"

class DXViewerApp
{
public:
    DXViewerApp(HINSTANCE instance);
    ~DXViewerApp();

    bool Init();
    void Run();
    unsigned int& width() { return width_; }
    unsigned int& height() { return height_; }
    LRESULT OnActivate(UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT OnSize(UINT msg, WPARAM wParam, LPARAM lParam);

private:
    HWND CreateViewerWindow(HINSTANCE instance, const unsigned int width, const unsigned int height);
    void UpdateScene();
    void DrawScene();

private:
    HINSTANCE instance_ = NULL;
    HWND window_ = NULL;
    unsigned int width_ = 800;
    unsigned int height_ = 600;
    bool paused_ = true;

    GraphicEngine engine_;
};

