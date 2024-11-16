#pragma once

#include <GLFW/glfw3.h>
#include <memory>

class Window
{
public:
    Window(size_t width, size_t height, const char *title);
    ~Window() = default;

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    void run();

    size_t getWidth() const;
    size_t getHeight() const;

private:
    size_t width_, height_;
    const char *title_;
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window_{
        nullptr, glfwDestroyWindow};

    void init();
};
