
#include <shader.hpp>

glm::vec4 vertexShader(const Vertex& vertex, const Uniforms& uniforms) {
    // clip pos
    glm::vec4 clipPos = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.pos, 1.f);

    // fragment pos
    float w = clipPos.w;
    glm::vec4 NDC = clipPos / w;
    NDC.w = 1.f / w;
    glm::vec4 fragPos = NDC;
    fragPos.x = (NDC.x + 1.f) * .5f * uniforms.screenWight;
    fragPos.y = (NDC.y + 1.f) * .5f * uniforms.screenHeight;

    return fragPos;
}

glm::vec3 fragmentShader(const glm::vec4& fragPos, const Uniforms& uniforms) {
    return glm::vec3(1.f);
}