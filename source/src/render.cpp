#include <model.hpp>
#include <render.hpp>

void Render::draw(FrameBuffer& fb,
                  Uniforms& uniforms,
                  const Shader& shader,
                  const std::unique_ptr<Model>& model) {

    const auto& vertices = model->getVertices();

    for (const auto& tri : model->getTriangles()) {

        size_t vertexIndex = 0;
        for (const auto& index : tri.indices) {
            const auto& vertex = vertices[index];
            getFragMesh()->screenMesh[vertexIndex] = shader.getVertexShader()(vertex, uniforms);
            vertexIndex++;
        }

        // rasterization
        rasterization(fb, shader, uniforms);
    }
}

void Render::rasterization(FrameBuffer& fb,
                           const Shader& shader,
                           const Uniforms& uniforms) {
    // bounding box
    setBBox({0, 0, uniforms.screenWidth, uniforms.screenHeight});
    getBBox().updateBBox(*getFragMesh());

    for (int x = getBBox().getMinX(); x < getBBox().getMaxX(); x++) {
        for (int y = getBBox().getMinY(); y < getBBox().getMaxY(); y++) {
            glm::vec2 screenPoint(x + .5f, y + .5f);
            glm::vec3 weights = calculateWeights(*getFragMesh(), screenPoint);

            // check if the point is inside the triangle
            if (weights.x < EPSILON || weights.y < EPSILON || weights.z < EPSILON) {
                continue;
            }

            // fragment shader
            glm::vec3 color = shader.getFragmentShader()(glm::vec4(x, y, 0.f, 1.f), uniforms);
            fb.setPixel(x, y, color);
        }
    }
}

glm::vec3 Render::calculateWeights(const FragMesh& fragMesh,
                                   const glm::vec2& screenPoint) {
    glm::vec3 weights(0.f), screenWeights(0.f);
    glm::vec4 fragCoords[3] = { getFragMesh()->screenMesh[0],
                               getFragMesh()->screenMesh[1],
                               getFragMesh()->screenMesh[2] };
    glm::vec2 ab = fragCoords[1] - fragCoords[0];
    glm::vec2 ac = fragCoords[2] - fragCoords[0];
    glm::vec2 ap = screenPoint - glm::vec2(fragCoords[0]);
    float factor = 1.0f / (ab.x * ac.y - ab.y * ac.x);
    float s = (ac.y * ap.x - ac.x * ap.y) * factor;
    float t = (ab.x * ap.y - ab.y * ap.x) * factor;
    screenWeights[0] = 1 - s - t;
    screenWeights[1] = s;
    screenWeights[2] = t;

    float w0 = fragCoords[0].w * screenWeights[0];
    float w1 = fragCoords[1].w * screenWeights[1];
    float w2 = fragCoords[2].w * screenWeights[2];
    float normalizer = 1.0f / (w0 + w1 + w2);
    weights[0] = w0 * normalizer;
    weights[1] = w1 * normalizer;
    weights[2] = w2 * normalizer;

    return weights;
}
