#include <application.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Application::run() {
    // render loop
    while (!glfwWindowShouldClose(window->getWindowPtr())) {

        // draw
        render->draw(*framebuffer, *uniforms, *shader, model);

        // 设置像素操作参数
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // drawFrameBuffer(fb);

        glDrawPixels(framebuffer->getWidth(), framebuffer->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, framebuffer->getScreenBuffer()->data());
        glfwSwapInterval(1);
        glfwSwapBuffers(window->getWindowPtr());

        glfwPollEvents();
    }
    glfwTerminate();
}

void Application::init() {
    size_t width = 800, height = 600;
    model = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "box.obj");
    window = std::make_unique<Window>(width, height, "ZBufRender");
    shader = std::make_unique<Shader>(vertexShader, fragmentShader);
    render = std::make_unique<Render>();
    framebuffer = std::make_shared<FrameBuffer>(width, height);
    
    // set uniforms and context
    glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
                              glm::vec3(0.0f),
                              glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 P = glm::perspective(glm::radians(45.0f),
                                   static_cast<float>(width) / static_cast<float>(height),
                                   0.1f,
                                   100.0f);

    uniforms = std::make_shared<Uniforms>(M, V, P, height, width);

    window->getContext().fb = framebuffer.get();
    window->getContext().uniforms = uniforms.get();
}