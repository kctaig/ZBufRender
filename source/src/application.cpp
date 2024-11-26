#include <application.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

void Application::run() const {
    // render loop
    while (!glfwWindowShouldClose(window->getWindowPtr())) {
        auto start = std::chrono::high_resolution_clock::now();

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

        auto end = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Execution time: " << duration << " ms" << std::endl;
    }
    glfwTerminate();
}

void Application::init() {
    size_t width = 600, height = 400;
    model = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "bunny.obj");
    // armadillo
    //model->modelInfo();
    window = std::make_unique<Window>(width, height, "ZBufRender");
    shader = std::make_unique<Shader>(vertexShader, fragmentShader);
    framebuffer = std::make_shared<FrameBuffer>(width, height);

    // set uniforms and context
    auto M = glm::mat4(1.0f);
    glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, 7.0f),
                              glm::vec3(0.0f),
                              glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 P = glm::perspective(glm::radians(45.0f),
                                   static_cast<float>(width) / static_cast<float>(height),
                                   0.1f,
                                   100.0f);

    uniforms = std::make_shared<Uniforms>(M, V, P,
                                          static_cast<int>(height),
                                          static_cast<int>(width));

    Window::getContext().fb = framebuffer.get();
    Window::getContext().uniforms = uniforms.get();
}
