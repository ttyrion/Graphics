#include "stdafx.h"
#include "resource.h"
#include "DXViewerApp.h"

DXViewerApp* theApp = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_ACTIVATE: {        
        theApp->OnActivate(message, wParam, lParam);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择: 
        switch (wmId)
        {
        case IDM_ABOUT:
            //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_SIZE: {
        theApp->width() = LOWORD(lParam);
        theApp->height() = HIWORD(lParam);
        theApp->OnSize(message, wParam, lParam);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

DXViewerApp::DXViewerApp(HINSTANCE instance) {
    instance_ = instance;
}

DXViewerApp::~DXViewerApp() {

}

bool DXViewerApp::Init() {
    window_ = CreateViewerWindow(instance_, width_, height_);
    if (!window_) {
        return false;
    }
    if (!engine_.InitDirect3D(window_, width_, height_)) {
        return false;
    }

    ::ShowWindow(window_, SW_SHOW);
    ::UpdateWindow(window_);

    return true;
}

void DXViewerApp::Run() {
    MSG msg;
    // 主消息循环: 
    while (true) {
        if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }

            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else {
            //消息队列没有消息
            UpdateScene();
            DrawScene();

            Sleep(100);
        }        
    }
}

LRESULT DXViewerApp::OnActivate(UINT msg, WPARAM wParam, LPARAM lParam) {
    if (LOWORD(wParam) == WA_INACTIVE) {
        paused_ = true;
    }
    else {
        paused_ = false;
    }

    return 0;
}

LRESULT DXViewerApp::OnSize(UINT msg, WPARAM wParam, LPARAM lParam) {
    engine_.ReSizeRender(width_, height_);

    return 0;
}

void DXViewerApp::UpdateScene() {
    engine_.UpdateScene();
}

void DXViewerApp::DrawScene() {
    engine_.DrawScene();
}

HWND DXViewerApp::CreateViewerWindow(HINSTANCE instance, const unsigned int width, const unsigned int height) {
    WNDCLASSEXW wcex;
    const LPCWSTR window_class = L"DXViewerClass";
    const LPCWSTR window_title = L"DXViewer";
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = instance;
    wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_DXVIEWER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DXVIEWER);
    wcex.lpszClassName = window_class;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    ATOM ca = RegisterClassExW(&wcex);
    if (!ca) {
        return NULL;
    }

    return CreateWindowW(window_class, window_title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, instance, nullptr);
}