#include "Window.hpp"
#include <iostream>

Window::Window(size_t width, size_t height, const char *title)
    : width_(width), height_(height), title_(title)
{
    init();
}

void Window::run()
{
    while (!glfwWindowShouldClose(window_.get()))
    {
        // TODO:äÖÈ¾Âß¼­

        glfwSwapBuffers(window_.get());
        glfwPollEvents();
    }
}

size_t Window::getWidth() const { return width_; }
size_t Window::getHeight() const { return height_; }

void Window::init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    window_.reset(glfwCreateWindow(width_, height_, title_, nullptr, nullptr));
    if (!window_)
    {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window_.get());
}