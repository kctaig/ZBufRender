#pragma once

#include <glm/glm.hpp>
#include <model.hpp>
#include <memory>

struct FragMesh {
    std::unique_ptr<glm::vec4[]> screenMesh;
    size_t vertexNum;
};

struct Uniforms {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    int screenWight, screenHeight;
};

class Shader {
   public:
    using vertexShader = glm::vec4 (*)(const Vertex&, const Uniforms&);
    using fragmentShader = glm::vec3 (*)(const glm::vec4&, const Uniforms&);

    Shader(const vertexShader& vs, const fragmentShader& fs)
        : vs(vs), fs(fs) {}

    vertexShader getVertexShader() const { return vs; }
    fragmentShader getFragmentShader() const { return fs; }

   private:
    vertexShader vs;
    fragmentShader fs;
};

glm::vec4 vertexShader(const Vertex& vertex, const Uniforms& uniforms);
glm::vec3 fragmentShader(const glm::vec4& fragPos, const Uniforms& uniforms);
