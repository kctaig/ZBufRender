#include "window.hpp"

curContext Window::context;

Window::Window(const size_t& width, const size_t& height, const char* title)
	: width(width), height(height), title(title) {
	init();
	lastX = static_cast<float>(width) / 2;
	lastY = static_cast<float>(height) / 2;
	context.lastXPtr = &lastX;
	context.lastYPtr = &lastY;
	context.firstMousePtr = &firstMouse;
	context.deltaTimePtr = &deltaTime;
	context.lastFramePtr = &lastFrame;
	context.useMousePtr = &useMouse;
}

void Window::framebufferCallback(GLFWwindow* window, int width, int height) {
	context.bufferPtr->bufferResize(width, height, glm::vec3(0));
	context.uniformsPtr->screenHeight = height;
	context.uniformsPtr->screenWidth = width;
}

void Window::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		//glfwSetWindowShouldClose(window, true);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // ÊÍ·ÅÊó±ê
	}
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		context.cameraPtr->ProcessKeyboard(FORWARD, *(context.deltaTimePtr));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		context.cameraPtr->ProcessKeyboard(BACKWARD, *(context.deltaTimePtr));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		context.cameraPtr->ProcessKeyboard(LEFT, *(context.deltaTimePtr));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		context.cameraPtr->ProcessKeyboard(RIGHT, *(context.deltaTimePtr));
}

void Window::mouseCallback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	if (context.useMousePtr && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		const auto xPos = static_cast<float>(xPosIn);
		const auto yPos = static_cast<float>(yPosIn);

		if (*(context.firstMousePtr))
		{
			*(context.lastXPtr) = xPos;
			*(context.lastYPtr) = yPos;
			*(context.firstMousePtr) = false;
		}

		const float xOffset = *(context.lastXPtr) - xPos;
		const float yOffset = yPos - *(context.lastYPtr); // reversed since y-coordinates go from bottom to top

		*(context.lastXPtr) = xPos;
		*(context.lastYPtr) = yPos;

		context.cameraPtr->ProcessMouseMovement(xOffset, yOffset);
	}
}

void Window::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	if (!context.useMousePtr) return;
	context.cameraPtr->ProcessMouseScroll(static_cast<float>(yOffset));
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
	//glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}
}