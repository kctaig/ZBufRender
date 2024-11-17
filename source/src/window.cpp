#include "Window.hpp"
#include <iostream>
#include <framebuffer.hpp>
#include <wtypes.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

Window::Window(size_t width, size_t height, const char *title)
    : width_(width), height_(height), title_(title)
{
    init();
}

void Window::drawFrameBuffer(const FrameBuffer &fb)
{
    HWND hwnd = glfwGetWin32Window(window_.get());
    HDC hdcWindow = GetDC(hwnd);
    if (!hdcWindow) {
        std::cerr << "Failed to get device context" << std::endl;
        return;
    }

    // ��ȡ FrameBuffer �ʹ��ڳߴ�
    const int fbHeight = static_cast<int>(fb.getHeight());
    const int fbWidth = static_cast<int>(fb.getWidth());
    const int height = static_cast<int>(min(height_, fbHeight));
    const int width = static_cast<int>(min(width_, fbWidth));

    // ���� BITMAPINFO
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // ʹ�ø�ֵ��ʾ���϶��µ� DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24; // ÿ���� 24 λ
    bmi.bmiHeader.biCompression = BI_RGB;

    // ���� DIB Section
    void* bits = nullptr;
    HBITMAP hbm = CreateDIBSection(hdcWindow, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hbm || !bits) {
        ReleaseDC(hwnd, hdcWindow);
        std::cerr << "Failed to create DIB section" << std::endl;
        return;
    }

    // �������� DC ��ѡ�� DIB
    HDC hdcMemory = CreateCompatibleDC(hdcWindow);
    if (!hdcMemory) {
        DeleteObject(hbm);
        ReleaseDC(hwnd, hdcWindow);
        std::cerr << "Failed to create compatible DC" << std::endl;
        return;
    }
    HGDIOBJ oldObject = SelectObject(hdcMemory, hbm);

    // ����������ݵ� DIB
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            glm::vec3 color = fb.getColor(x, y);
            int index = y * width + x;
            // ʹ�� BGR ˳��洢
            ((unsigned char*)bits)[index * 3 + 0] = static_cast<unsigned char>(color.b * 255);
            ((unsigned char*)bits)[index * 3 + 1] = static_cast<unsigned char>(color.g * 255);
            ((unsigned char*)bits)[index * 3 + 2] = static_cast<unsigned char>(color.r * 255);
        }
    }

    // �� DIB ���ݻ��Ƶ�����
    BitBlt(hdcWindow, 0, 0, width, height, hdcMemory, 0, 0, SRCCOPY);

    // �ͷ���Դ
    SelectObject(hdcMemory, oldObject);
    DeleteDC(hdcMemory);
    DeleteObject(hbm);
    ReleaseDC(hwnd, hdcWindow);
}

void Window::run()
{
    // todo: loade scene

    while (!glfwWindowShouldClose(window_.get()))
    {
        FrameBuffer fb(width_, height_);
        for (size_t y = 0; y < fb.getHeight(); ++y)
        {
            for (size_t x = 0; x < fb.getWidth(); ++x)
            {
                fb.setPixel(x, y, glm::vec3(1.0f, 0.f, 0.0f));
            }
        }

        drawFrameBuffer(fb);
        glfwPollEvents();
    }
    glfwTerminate();
}

void Window::init()
{

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // ��ʹ�� OpenGL

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    window_.reset(glfwCreateWindow(static_cast<int>(width_), static_cast<int>(height_), title_, nullptr, nullptr));
    if (!window_)
    {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window_.get());
}