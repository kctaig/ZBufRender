#pragma once

#include <bbox.hpp>
#include <frame_buffer.hpp>

class Render {
public:
    Render() = default;

    ~Render() = default;

    static void processTriangles(FrameBuffer &fb,
                                 const Uniforms &uniforms,
                                 const Shader &shader,
                                 const std::unique_ptr<Model> &model,
                                 bool useParallel = true);

    // ÒÑ·ÏÆú
    static void draw(FrameBuffer &fb,
                     const Uniforms &uniforms,
                     const Shader &shader,
                     const std::unique_ptr<Model> &model);

    static void rasterization(FrameBuffer &fb,
                              const Shader &shader,
                              const Uniforms &uniforms,
                              const FragMesh &fragMesh);

    static glm::vec3 calculateWeights(
        const FragMesh &fragMesh,
        const glm::vec2 &screenPoint);

    static constexpr float EPSILON = std::numeric_limits<float>::epsilon();
};
