#include <application.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

void Application::run() const {
    // render loop
    while (!glfwWindowShouldClose(window->getWindowPtr())) {
        // auto start = std::chrono::high_resolution_clock::now();
        // 设置时间
        framebuffer->clear();
        float curFrame = static_cast<float>(glfwGetTime());
        window->deltaTime = curFrame - window->lastFrame;
        window->lastFrame = curFrame;

        Window::processInput(window->getWindowPtr());
        // 更新MVP矩阵
        uniforms->updateMVP(*camera,*framebuffer);

        Render::processTriangles(*framebuffer,
                                 *uniforms,
                                 *shader,
                                 model, false);

        // 设置像素操作参数
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glDrawPixels(static_cast<GLsizei>(framebuffer->getWidth()),
                     static_cast<GLsizei>(framebuffer->getHeight()),
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     framebuffer->getScreenBuffer()->data());

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glfwSwapInterval(1);
        glfwSwapBuffers(window->getWindowPtr());

        glfwPollEvents();

        // auto end = std::chrono::high_resolution_clock::now();
        // const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // std::cout << "Execution time: " << duration << " ms" << std::endl;
    }
    glfwTerminate();
}

void Application::init() {
    size_t width = 600, height = 400;
    model = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "bunny.obj");// armadillo
    window = std::make_unique<Window>(width, height, "ZBufRender");
    shader = std::make_unique<Shader>(vertexShader, fragmentShader);
    framebuffer = std::make_shared<FrameBuffer>(width, height);
    camera = std::make_unique<Camera>(glm::vec3(0,0,6));
    uniforms = std::make_unique<Uniforms>();

    Window::getContext().fb = framebuffer.get();
    Window::getContext().uniforms = uniforms.get();
    Window::getContext().camera = camera.get();
}
