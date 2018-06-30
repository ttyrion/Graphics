#pragma once
class GraphicEngine
{
public:
    GraphicEngine();
    ~GraphicEngine();

    void InitDirect3D(HWND render_window, const unsigned int width, const unsigned int height);
};

