#pragma once

#include "bbox.hpp"
#include "buffer.hpp"
#include "scan_line_buffer.hpp"

enum RasterType { REGULAR, SCANLINE };

class Render {
public:
    Render() = default;

    ~Render() = default;

    Render(std::unique_ptr<Model> mPtr,
           std::shared_ptr<Camera> cPtr,
           std::shared_ptr<ZBuffer> fbPtr,
           RasterType type) {
        modelPtr = std::move(mPtr);
        cameraPtr = cPtr;
        bufferPtr = fbPtr;
        rasterType = type;
    }

    void processTriangles(const Uniforms &uniforms,
                          const Shader &shader,
                          bool useParallel = true) const;

    // ÒÑ·ÏÆú
    // void draw(const Uniforms &uniforms,
    //           const Shader &shader) const;

    void regularRaster(const FragMesh &fragMesh,
                       const Shader &shader,
                       const Uniforms &uniforms) const;

    void scanLineRender(const Shader &shader,
                        const Uniforms &uniforms) const;

    static glm::vec3 calculateWeights(
        const FragMesh &fragMesh,
        const glm::vec2 &screenPoint);

    void renderPixel(glm::ivec2 pixel,
                     const FragMesh &fragMesh,
                     const Shader &shader,
                     const Uniforms &uniforms) const;

    static constexpr float EPSILON = std::numeric_limits<float>::epsilon();

    auto getCameraPtr() const { return cameraPtr; }
    auto getBufferPtr() const { return bufferPtr; }
    auto getModelPtr() const { return modelPtr.get(); }

private:
    std::unique_ptr<Model> modelPtr;
    std::shared_ptr<Camera> cameraPtr;
    std::shared_ptr<ZBuffer> bufferPtr;
    RasterType rasterType;
};
