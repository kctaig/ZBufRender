#include <glad/glad.h>
#include "Window.hpp"
#include <algorithm>
#include <iostream>
#include <render.hpp>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <glm/gtc/matrix_transform.hpp>


curContext Window::context;

Window::Window(const size_t &width, const size_t &height, const char *title)
    : width(width), height(height), title(title) {
    init();
    lastX = static_cast<float>(width) / 2;
    lastY = static_cast<float>(height) / 2;
    context.lastX = &lastX;
    context.lastY = &lastY;
    context.firstMouse = &firstMouse;
    context.deltaTime = &deltaTime;
    context.lastFrame = &lastFrame;
}

void Window::framebufferCallback(GLFWwindow *window, int width, int height) {
    context.fb->reCreate(width, height, glm::vec3(0));
    context.uniforms->screenHeight = height;
    context.uniforms->screenWidth = width;
}

void Window::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        //glfwSetWindowShouldClose(window, true);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // 释放鼠标
    }
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        context.camera->ProcessKeyboard(FORWARD, *(context.deltaTime));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        context.camera->ProcessKeyboard(BACKWARD, *(context.deltaTime));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        context.camera->ProcessKeyboard(LEFT, *(context.deltaTime));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        context.camera->ProcessKeyboard(RIGHT, *(context.deltaTime));
}

void Window::mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (*(context.firstMouse))
    {
        *(context.lastX) = xpos;
        *(context.lastY) = ypos;
        *(context.firstMouse) = false;
    }

    float xoffset = xpos - *(context.lastX);
    float yoffset = *(context.lastY) - ypos; // reversed since y-coordinates go from bottom to top

    *(context.lastX) = xpos;
    *(context.lastY) = ypos;

    context.camera->ProcessMouseMovement(xoffset, yoffset);
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    context.camera->ProcessMouseScroll(static_cast<float>(yoffset));
}


void Window::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    window.reset(glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title, nullptr, nullptr));
    if (!window) {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window\n";
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window.get());
    glfwSetFramebufferSizeCallback(window.get(), framebufferCallback);
    glfwSetCursorPosCallback(window.get(), mouseCallback);
    glfwSetScrollCallback(window.get(), scrollCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Window::drawFrameBuffer(const FrameBuffer &fb) const {
    HWND hwnd = glfwGetWin32Window(window.get());
    HDC hdcWindow = GetDC(hwnd);
    if (!hdcWindow) {
        std::cerr << "Failed to get device context" << std::endl;
        return;
    }

    // 获取 FrameBuffer 和窗口尺寸
    const int fbHeight = static_cast<int>(fb.getHeight());
    const int fbWidth = static_cast<int>(fb.getWidth());
    const int screenheight = (min(static_cast<int>(height), fbHeight));
    const int screenwidth = (min(static_cast<int>(width), fbWidth));

    // 设置 BITMAPINFO
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = screenwidth;
    bmi.bmiHeader.biHeight = -screenheight; // 使用负值表示自上而下的 DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24; // 每像素 24 位
    bmi.bmiHeader.biCompression = BI_RGB;

    // 创建 DIB Section
    void *bits = nullptr;
    HBITMAP hbm = CreateDIBSection(hdcWindow, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hbm || !bits) {
        ReleaseDC(hwnd, hdcWindow);
        std::cerr << "Failed to create DIB section" << std::endl;
        return;
    }

    // 创建兼容 DC 并选入 DIB
    HDC hdcMemory = CreateCompatibleDC(hdcWindow);
    if (!hdcMemory) {
        DeleteObject(hbm);
        ReleaseDC(hwnd, hdcWindow);
        std::cerr << "Failed to create compatible DC\n";
        return;
    }
    HGDIOBJ oldObject = SelectObject(hdcMemory, hbm);

    // 填充像素数据到 DIB
    for (int y = 0; y < screenheight; ++y) {
        for (int x = 0; x < screenwidth; ++x) {
            const glm::vec3 color = fb.getColor(x, y);
            const int index = y * screenwidth + x;
            // 使用 BGR 顺序存储
            static_cast<unsigned char *>(bits)[index * 3 + 0] = static_cast<unsigned char>(color.b * 255);
            static_cast<unsigned char *>(bits)[index * 3 + 1] = static_cast<unsigned char>(color.g * 255);
            static_cast<unsigned char *>(bits)[index * 3 + 2] = static_cast<unsigned char>(color.r * 255);
        }
    }

    // 将 DIB 内容绘制到窗口
    BitBlt(hdcWindow, 0, 0, screenwidth, screenheight, hdcMemory, 0, 0, SRCCOPY);

    // 释放资源
    SelectObject(hdcMemory, oldObject);
    DeleteDC(hdcMemory);
    DeleteObject(hbm);
    ReleaseDC(hwnd, hdcWindow);
}
