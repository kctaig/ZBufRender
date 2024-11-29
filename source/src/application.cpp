
#include <chrono>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "application.hpp"

void Application::run() const {
    // render loop
    while (!glfwWindowShouldClose(windowPtr->getWindowPtr())) {
        // auto start = std::chrono::high_resolution_clock::now();

        // set time
        renderPtr->getBufferPtr()->clear(glm::vec3(0));
        const auto curFrame = static_cast<float>(glfwGetTime());
        windowPtr->deltaTime = curFrame - windowPtr->lastFrame;
        windowPtr->lastFrame = curFrame;

        // get input
        Window::processInput(windowPtr->getWindowPtr());

        uniformsPtr->updateMVP(*renderPtr->getCameraPtr(), *renderPtr->getBufferPtr());
        renderPtr->processTriangles(*uniformsPtr,
                                    *shaderPtr,
                                    true);

        // 设置像素操作参数
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glDrawPixels(static_cast<GLsizei>(renderPtr->getBufferPtr()->getWidth()),
                     static_cast<GLsizei>(renderPtr->getBufferPtr()->getHeight()),
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     renderPtr->getBufferPtr()->getColorsPtr()->data());

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // glfwSwapInterval(1);
        glfwSwapBuffers(windowPtr->getWindowPtr());

        glfwPollEvents();

        // auto end = std::chrono::high_resolution_clock::now();
        // const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // std::cout << "Execution time: " << duration << " ms" << std::endl;
    }
    glfwTerminate();
}

void Application::init() {
    size_t width = 600, height = 400;
    auto modelPtr = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "bunny.obj"); // armadillo
    auto cameraPtr = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 6.0f));
    auto bufferPtr = std::make_shared<Buffer>(width, height);

    renderPtr = std::make_unique<Render>(std::move(modelPtr),cameraPtr,bufferPtr);
    windowPtr = std::make_unique<Window>(width, height, "ZBufRender");
    shaderPtr = std::make_unique<Shader>(vertexShader, fragmentShader);
    uniformsPtr = std::make_unique<Uniforms>();

    Window::getContext().bufferPtr = bufferPtr.get();
    Window::getContext().uniformsPtr = uniformsPtr.get();
    Window::getContext().cameraPtr = cameraPtr.get();
}
