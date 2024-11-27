#pragma once

#include <GLFW/glfw3.h>
#include <frame_buffer.hpp>
#include <shader.hpp>

#include "Camera.hpp"

struct curContext {
    FrameBuffer *fb;
    Uniforms *uniforms;
    Camera *camera;
    float *lastX, *lastY;
    bool *firstMouse;
    float *deltaTime, *lastFrame;
};

class Window {
public:
    Window() = default;

    ~Window() = default;

    Window(const Window &) = delete;

    Window &operator=(const Window &) = delete;

    Window(const size_t &width, const size_t &height, const char *title);

    void drawFrameBuffer(const FrameBuffer &fb) const;

    size_t getWidth() const { return width; }
    size_t getHeight() const { return height; }
    GLFWwindow *getWindowPtr() const { return window.get(); }

    static void processInput(GLFWwindow *window);
    static void framebufferCallback(GLFWwindow *window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    static curContext &getContext() { return context; }

    float lastX, lastY;
    bool firstMouse = true;
    float deltaTime = 0.0f, lastFrame = 0.0f;

private:
    size_t width{}, height{};
    const char *title{};
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window{
        nullptr, glfwDestroyWindow
    };
    static curContext context;

    void init();
};
