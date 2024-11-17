#pragma once

#include <GLFW/glfw3.h>
#include <memory>
#include <shader.hpp>
#include <scene.hpp>
#include <framebuffer.hpp>

class Window
{
public:
    Window(size_t width, size_t height, const char *title);
    ~Window() = default;

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    void drawFrameBuffer(const FrameBuffer &fb);
    void run();

    size_t getWidth() const { return width_; }
    size_t getHeight() const { return height_; }

private:
    size_t width_, height_;
    const char *title_;
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window_{
        nullptr, glfwDestroyWindow};

    void init();
};
