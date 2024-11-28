#include <application.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

void Application::run() const {
    // render loop
    while (!glfwWindowShouldClose(windowPtr->getWindowPtr())) {
        // auto start = std::chrono::high_resolution_clock::now();

        // 设置时间
        renderPtr->getFrameBufferPtr()->clear();
        const auto curFrame = static_cast<float>(glfwGetTime());
        windowPtr->deltaTime = curFrame - windowPtr->lastFrame;
        windowPtr->lastFrame = curFrame;

        Window::processInput(windowPtr->getWindowPtr());
        // 更新MVP矩阵
        uniformsPtr->updateMVP(*renderPtr->getCameraPtr(), *renderPtr->getFrameBufferPtr());

        renderPtr->processTriangles(*uniformsPtr,
                                    *shaderPtr,
                                    false);

        // 设置像素操作参数
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glDrawPixels(static_cast<GLsizei>(renderPtr->getFrameBufferPtr()->getWidth()),
                     static_cast<GLsizei>(renderPtr->getFrameBufferPtr()->getHeight()),
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     renderPtr->getFrameBufferPtr()->getScreenBuffer()->data());

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glfwSwapInterval(1);
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
    auto framebufferPtr = std::make_shared<FrameBuffer>(width, height);

    renderPtr = std::make_unique<Render>(std::move(modelPtr),cameraPtr,framebufferPtr);
    windowPtr = std::make_unique<Window>(width, height, "ZBufRender");
    shaderPtr = std::make_unique<Shader>(vertexShader, fragmentShader);
    uniformsPtr = std::make_unique<Uniforms>();

    Window::getContext().fb = framebufferPtr.get();
    Window::getContext().uniforms = uniformsPtr.get();
    Window::getContext().camera = cameraPtr.get();
}
