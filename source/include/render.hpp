#pragma once

#include <bbox.hpp>
#include <frame_buffer.hpp>


#define EPSILON 1e-5f

class Render {
   public:
       Render() = default;
	   ~Render() = default;

    void processTriangles(FrameBuffer& fb,
              Uniforms& uniforms,
              const Shader& shader,
              const std::unique_ptr<Model>& model, 
              bool useParallel = true);

    // ÒÑ·ÏÆú
    void Render::draw(FrameBuffer& fb,
        Uniforms& uniforms,
        const Shader& shader,
        const std::unique_ptr<Model>& model);

    void rasterization(FrameBuffer& fb,
                        const Shader& shader,
                        const Uniforms& uniforms,
                        const FragMesh& fragMesh);

    glm::vec3 calculateWeights(
        const FragMesh& fragMesh,
        const glm::vec2& screenPoint);

};
