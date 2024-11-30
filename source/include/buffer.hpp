#pragma once

#include <glm/glm.hpp>
#include <memory>

class ZBuffer {
public:
    ZBuffer() = default;

    virtual ~ZBuffer() = default;

    ZBuffer(size_t width, size_t height): width(width), height(height) {
    }

    size_t getWidth() const { return width; }
    size_t getHeight() const { return height; }

    auto &getPixelPtr() { return pixelPtr; }
    auto &getDepthPtr() { return depthPtr; }

    glm::u8vec3 toU8Vec3(const glm::vec3 &color);

    virtual void clear(glm::vec3 color) = 0;

    virtual void bufferResize(size_t w, size_t h, glm::vec3 color) = 0;

protected:
    size_t width{}, height{};
    std::unique_ptr<std::vector<float> > depthPtr{};
    std::unique_ptr<std::vector<glm::u8vec3> > pixelPtr{};
};


class RegularZBuffer : public ZBuffer {
public:
    RegularZBuffer() = default;

    RegularZBuffer(size_t width, size_t height);

    const auto getIndex(size_t x, size_t y) const { return y * width + x; };
    const auto &getDepth(size_t x, size_t y) const { return (*depthPtr)[getIndex(x, y)]; }
    const auto &getColor(size_t x, size_t y) const { return (*pixelPtr)[getIndex(x, y)]; }

    void setDepth(size_t x, size_t y, float depth) { (*depthPtr)[getIndex(x, y)] = depth; }
    void setPixel(size_t x, size_t y, glm::vec3 color) { (*pixelPtr)[getIndex(x, y)] = toU8Vec3(color); }

    void clear(glm::vec3 color) override;

    void bufferResize(size_t w, size_t h, glm::vec3 color) override;

private:
    size_t pixelCount{};
};
