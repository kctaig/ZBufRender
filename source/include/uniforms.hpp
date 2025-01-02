#pragma once

#include <ext/matrix_clip_space.hpp>
#include "camera.hpp"

struct Uniforms {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view;
    glm::mat4 projection;

    int screenWidth;
    int screenHeight;

    Uniforms() = default;
    ~Uniforms() = default;

    Uniforms(const glm::mat4& m, const glm::mat4& v, const glm::mat4& p, const int h, const int w)
        : model(m), view(v), projection(p), screenWidth(w), screenHeight(h) {
    }

    inline void updateMVP(const Camera& cam, const size_t w, const size_t h);
    void updateModel(const glm::mat4& m) { model = m; }
};

void Uniforms::updateMVP(const Camera& cam, const size_t w, const size_t h) {
    screenWidth = static_cast<int>(w);
    screenHeight = static_cast<int>(h);
    view = cam.GetViewMatrix();
    projection = glm::perspective(glm::radians(cam.Zoom),
                                  static_cast<float>(screenWidth) / static_cast<float>(screenHeight),
                                  1.f,
                                  100.0f);
}