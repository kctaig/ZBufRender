#pragma once

#include <render.hpp>
#include <window.hpp>

class Application {
public:
    Application() { init(); }

    ~Application() = default;

    void run() const;

private:
    std::unique_ptr<Window> windowPtr;
    std::unique_ptr<Shader> shaderPtr;
    std::shared_ptr<Uniforms> uniformsPtr;
    std::unique_ptr<Render> renderPtr;
    // std::shared_ptr<FrameBuffer> framebuffer;

    void init();
};
