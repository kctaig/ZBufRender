#pragma once

#include <bbox.hpp>
#include <frame_buffer.hpp>

class Render {
public:
    Render() = default;

    ~Render() = default;

    Render(std::unique_ptr<Model> mPtr, std::shared_ptr<Camera> cPtr, std::shared_ptr<FrameBuffer> fbPtr) {
        modelPtr = std::move(mPtr);
        cameraPtr = cPtr;
        framebufferPtr = fbPtr;
    }

    void processTriangles(const Uniforms &uniforms,
                          const Shader &shader,
                          bool useParallel = true) const;

    // ÒÑ·ÏÆú
    void draw(const Uniforms &uniforms,
              const Shader &shader) const;

    void rasterization(const Shader &shader,
                       const Uniforms &uniforms,
                       const FragMesh &fragMesh) const;

    static glm::vec3 calculateWeights(
        const FragMesh &fragMesh,
        const glm::vec2 &screenPoint);

    static constexpr float EPSILON = std::numeric_limits<float>::epsilon();

    auto getCameraPtr() const { return cameraPtr; }
    auto getFrameBufferPtr() const { return framebufferPtr; }
    auto getModelPtr() const { return modelPtr.get(); }

private:
    std::unique_ptr<Model> modelPtr;
    std::shared_ptr<Camera> cameraPtr;
    std::shared_ptr<FrameBuffer> framebufferPtr;
};
