#include <GLFW/glfw3.h> 

#include "Window.hpp"
#include <wtypes.h>
#include <algorithm>
#include <iostream>
#include <render.hpp>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <glm/gtc/matrix_transform.hpp>
#include <gl/GL.h>


Window::curContext Window::context{};

Window::Window(size_t width, size_t height, const char* title)
    : width_(width), height_(height), title_(title) {
    init();
}

void Window::drawFrameBuffer(const FrameBuffer& fb) {
    HWND hwnd = glfwGetWin32Window(window_.get());
    HDC hdcWindow = GetDC(hwnd);
    if (!hdcWindow) {
        std::cerr << "Failed to get device context" << std::endl;
        return;
    }

    // 获取 FrameBuffer 和窗口尺寸
    const int fbHeight = static_cast<int>(fb.getHeight());
    const int fbWidth = static_cast<int>(fb.getWidth());
    const int height = static_cast<int>(min(height_, fbHeight));
    const int width = static_cast<int>(min(width_, fbWidth));

    // 设置 BITMAPINFO
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // 使用负值表示自上而下的 DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;  // 每像素 24 位
    bmi.bmiHeader.biCompression = BI_RGB;

    // 创建 DIB Section
    void* bits = nullptr;
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
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            glm::vec3 color = fb.getColor(x, y);
            int index = y * width + x;
            // 使用 BGR 顺序存储
            ((unsigned char*)bits)[index * 3 + 0] = static_cast<unsigned char>(color.b * 255);
            ((unsigned char*)bits)[index * 3 + 1] = static_cast<unsigned char>(color.g * 255);
            ((unsigned char*)bits)[index * 3 + 2] = static_cast<unsigned char>(color.r * 255);
        }
    }

    // 将 DIB 内容绘制到窗口
    BitBlt(hdcWindow, 0, 0, width, height, hdcMemory, 0, 0, SRCCOPY);

    // 释放资源
    SelectObject(hdcMemory, oldObject);
    DeleteDC(hdcMemory);
    DeleteObject(hbm);
    ReleaseDC(hwnd, hdcWindow);
}

void Window::run() {
    // load model
    std::unique_ptr<Model> model = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "cube.obj");
    //model->modelInfo();

    // init fb
    FrameBuffer fb(width_, height_);

    // set MVP
    glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
                              glm::vec3(0.0f),
                              glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 P = glm::perspective(glm::radians(45.0f),
                                   static_cast<float>(width_) / static_cast<float>(height_),
                                   0.1f,
                                   100.0f);
    // set Uniforms
    uniforms = Uniforms{M, V, P,
                      static_cast<int>(width_),
                      static_cast<int>(height_)};

    context.fb = &fb;
    context.curHeight = &height_;
    context.curWidth = &width_;
	context.uniforms = &uniforms;

    Render render{};

    // set shader
    Shader shader(vertexShader, fragmentShader);

    // render loop
    while (!glfwWindowShouldClose(window_.get())) {
        //fb.clear();

        // draw
        render.draw(fb, uniforms, shader, model);

        // 设置像素操作参数
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        //drawFrameBuffer(fb);

        glDrawPixels(uniforms.screenWidth, uniforms.screenHeight, GL_RGB, GL_UNSIGNED_BYTE, fb.getScreenBuffer()->data());
        //glfwSwapInterval(1);
        glfwSwapBuffers(window_.get());

        glfwPollEvents();
    }
    glfwTerminate();
}

void Window::framebufferCallback(GLFWwindow* window, int width, int height)
{
	*context.curHeight = height;
    *context.curWidth = width;
	context.fb->reCreate(width, height,glm::vec3(1,0,1));
	context.uniforms->screenWidth = width;
	context.uniforms->screenHeight = height;

    glm::mat4 P = glm::perspective(glm::radians(45.0f),
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        100.0f);
	context.uniforms->projection = P;
}


void Window::init() {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    window_.reset(glfwCreateWindow(static_cast<int>(width_), static_cast<int>(height_), title_, nullptr, nullptr));
    if (!window_) {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window\n";
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window_.get());

    //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //    std::cerr << "Failed to initialize GLAD" << std::endl;
    //    exit(EXIT_FAILURE);
    //}

    glfwSetFramebufferSizeCallback(window_.get(), framebufferCallback);
}