#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

#include "render.hpp"
#include "zbuffer.hpp"
#include "shader.hpp"
#include "camera.hpp"

struct curContext {
	ZBuffer* bufferPtr;
	Uniforms* uniformsPtr;
	Camera* cameraPtr;
	float* lastXPtr, * lastYPtr;
	bool* firstMousePtr;
	float* deltaTimePtr, * lastFramePtr;
	bool* useMousePtr;
};

class Window {
public:
	Window() = default;

	~Window() = default;

	Window(const Window&) = delete;

	Window& operator=(const Window&) = delete;

	Window(const size_t& width, const size_t& height, const char* title);

	size_t getWidth() const { return width; }
	size_t getHeight() const { return height; }
	GLFWwindow* getWindowPtr() const { return window.get(); }

	static void processInput(GLFWwindow* window);
	static void framebufferCallback(GLFWwindow* window, int width, int height);
	static void mouseCallback(GLFWwindow* window, double xPosIn, double yPosIn);
	static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

	static curContext& getContext() { return context; }

	float lastX, lastY;
	bool firstMouse = true;
	float deltaTime = 0.0f, lastFrame = 0.0f;
	bool useMouse = true;

private:
	size_t width{}, height{};
	const char* title{};
	std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window{
		nullptr, glfwDestroyWindow
	};
	static curContext context;

	void init();
};