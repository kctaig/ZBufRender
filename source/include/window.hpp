#pragma once

#include <GLFW/glfw3.h>
#include <frame_buffer.hpp>
#include <shader.hpp>

struct curContext {
    FrameBuffer* fb;
    Uniforms* uniforms;
};

class Window {
   public:
    Window() = default;
    ~Window() = default;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(size_t width, size_t height, const char* title);

    void drawFrameBuffer(const FrameBuffer& fb);

    size_t getWidth() const { return width; }
    size_t getHeight() const { return height; }
    GLFWwindow* getWindowPtr() { return window.get(); }

    static void framebufferCallback(GLFWwindow* window, int width, int height);
    curContext& getContext() { return context; }

   private:
    size_t width, height;
    const char* title;
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window{
        nullptr, glfwDestroyWindow};

    static curContext context;

    void init();
};
