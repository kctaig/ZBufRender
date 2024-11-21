#pragma once

#include <bbox.hpp>
#include <frame_buffer.hpp>


#define EPSILON 1e-5f

class Render {
   public:
       Render() = default;
	   ~Render() = default;

    void draw(FrameBuffer& fb,
              Uniforms& uniforms,
              const Shader& shader,
              const std::unique_ptr<Model>& model);
    void rasterization(FrameBuffer& fb,
                       const Shader& shader,
                       const Uniforms& uniforms,
                       const FragMesh& fragMesh);
    BBOX getBBox() const { return bbox; }
    void setBBox(const BBOX& bbox) { this->bbox = bbox; }

    glm::vec3 calculateWeights(
        const FragMesh& fragMesh,
        const glm::vec2& screenPoint);

   private:
    BBOX bbox;
};
