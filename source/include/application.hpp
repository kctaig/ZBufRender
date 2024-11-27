#pragma once

#include <render.hpp>
#include <window.hpp>

class Application {
public:
    Application() { init(); }

    ~Application() = default;

    void run() const;

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Render> render;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Model> model;
    std::shared_ptr<FrameBuffer> framebuffer;
    std::shared_ptr<Uniforms> uniforms;
    std::unique_ptr<Camera> camera;

    void init();
};
