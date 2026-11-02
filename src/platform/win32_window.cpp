#include "platform/window.h"

#include <string>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace {

    HWND g_hwnd = nullptr;
    HDC g_dc = nullptr;
    bool g_open = false;
    Input* g_input = nullptr;
    const Canvas* g_last_frame = nullptr;

    int map_key(WPARAM vk) {
        switch (vk) {
        case VK_LEFT: return KeyLeft;
        case VK_RIGHT: return KeyRight;
        case VK_UP: return KeyUp;
        case VK_DOWN: return KeyDown;
        case VK_SPACE: return KeySpace;
        case VK_RETURN: return KeyEnter;
        case VK_ESCAPE: return KeyEscape;
        case 'Z': return KeyZ;
        case 'X': return KeyX;
        case 'C': return KeyC;
        case 'P': return KeyP;
        case 'R': return KeyR;
        default:  return -1;
        }
    }

    void blit(HDC dc, const Canvas& canvas) {
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = canvas.width();
        bmi.bmiHeader.biHeight = -canvas.height();  // negative = top-down rows
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        StretchDIBits(dc,
            0, 0, canvas.width(), canvas.height(),
            0, 0, canvas.width(), canvas.height(),
            canvas.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);
    }

    LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        switch (msg) {
        case WM_CLOSE:
            g_open = false;
            return 0;

        case WM_KEYDOWN: {
            const int key = map_key(wp);
            if (key >= 0 && g_input != nullptr) {
                const bool was_down = (lp & (1 << 30)) != 0;  // bit 30 = auto-repeat
                if (!was_down) g_input->pressed[key] = true;
                g_input->down[key] = true;
            }
            return 0;
        }

        case WM_KEYUP: {
            const int key = map_key(wp);
            if (key >= 0 && g_input != nullptr) g_input->down[key] = false;
            return 0;
        }

        case WM_KILLFOCUS:
            if (g_input != nullptr) g_input->release_all();
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            if (g_last_frame != nullptr) blit(dc, *g_last_frame);
            EndPaint(hwnd, &ps);
            return 0;
        }
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }

} // namespace

namespace platform {

    bool open_window(const char* title, int width, int height) {
        HINSTANCE instance = GetModuleHandleW(nullptr);

        WNDCLASSW wc = {};
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = window_proc;
        wc.hInstance = instance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = L"TetrisWindowClass";
        if (!RegisterClassW(&wc)) return false;

        std::wstring wide_title;
        for (const char* p = title; *p != '\0'; ++p) {
            wide_title.push_back(static_cast<wchar_t>(*p));
        }

        const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        RECT rect = { 0, 0, width, height };
        AdjustWindowRect(&rect, style, FALSE);

        g_hwnd = CreateWindowW(wc.lpszClassName, wide_title.c_str(), style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left, rect.bottom - rect.top,
            nullptr, nullptr, instance, nullptr);
        if (g_hwnd == nullptr) return false;

        ShowWindow(g_hwnd, SW_SHOW);
        g_dc = GetDC(g_hwnd);
        g_open = true;
        return true;
    }

    bool pump_messages(Input& input) {
        g_input = &input;
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) g_open = false;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        return g_open;
    }

    void present(const Canvas& canvas) {
        g_last_frame = &canvas;
        if (g_dc != nullptr) blit(g_dc, canvas);
    }

    void close_window() {
        g_last_frame = nullptr;
        g_input = nullptr;
        if (g_dc != nullptr) {
            ReleaseDC(g_hwnd, g_dc);
            g_dc = nullptr;
        }
        if (g_hwnd != nullptr) {
            DestroyWindow(g_hwnd);
            g_hwnd = nullptr;
        }
    }

}  // namespace platform

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int) {
    return run_app();
}
