#pragma once

#include <glm/glm.hpp>
#include <model.hpp>
#include <memory>
#include <vector>
#include <ext/matrix_clip_space.hpp>

#include "Camera.hpp"
#include "frame_buffer.hpp"

struct FragMesh {
    std::vector<glm::vec4> screenMesh;
    size_t vertexNum;
};

struct Uniforms {
    Uniforms() = default;

    ~Uniforms() = default;

    Uniforms(const glm::mat4 &m, const glm::mat4 &v, const glm::mat4 &p, const int h, const int w)
        : model(m), view(v), projection(p), screenWidth(w), screenHeight(h) {
    }

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view;
    glm::mat4 projection;

    int screenWidth;
    int screenHeight;

    void updateMVP(const Camera &cam, const FrameBuffer &fb) {
        screenWidth = static_cast<int>(fb.getWidth());
        screenHeight = static_cast<int>(fb.getHeight());
        view = cam.GetViewMatrix();
        projection = glm::perspective(glm::radians(cam.Zoom),
                                      static_cast<float>(screenWidth) / static_cast<float>(screenHeight),
                                      0.1f,
                                      100.0f);
    }
};

class Shader {
public:
    Shader() = default;

    ~Shader() = default;

    using vertexShader = glm::vec4 (*)(const Vertex &, const Uniforms &);
    using fragmentShader = glm::vec3 (*)(const glm::vec4 &, const Uniforms &);

    Shader(const vertexShader &vs, const fragmentShader &fs)
        : vs(vs), fs(fs) {
    }

    vertexShader getVertexShader() const { return vs; }
    fragmentShader getFragmentShader() const { return fs; }

private:
    vertexShader vs;
    fragmentShader fs;
};

glm::vec4 vertexShader(const Vertex &vertex, const Uniforms &uniforms);

glm::vec3 fragmentShader(const glm::vec4 &fragPos, const Uniforms &uniforms);
