#pragma once

#include <glm/glm.hpp>
#include <memory>

class Buffer {
public:
    Buffer() = default;

    ~Buffer() = default;

    Buffer(size_t width, size_t height);

    size_t getWidth() const { return width; }
    size_t getHeight() const { return height; }

    size_t getIndex(size_t x, size_t y) const;

    float getDepth(size_t x, size_t y) const { return (*depthsPtr)[getIndex(x, y)]; }
    void setDepth(size_t x, size_t y, float depth) { (*depthsPtr)[getIndex(x, y)] = depth; }

    void setPixel(size_t x, size_t y, glm::vec3 color) { (*colorsPtr)[getIndex(x, y)] = toU8Vec3(color); }

    void clear(glm::vec3 color = {0, 0, 0});

    void bufferResize(size_t w, size_t h, glm::vec3 color = {0, 0, 0});

    const glm::u8vec3 &getColor(size_t x, size_t y) const { return (*colorsPtr)[getIndex(x, y)]; }

    const std::unique_ptr<std::vector<glm::u8vec3> > &getColorsPtr() { return colorsPtr; }

    static glm::u8vec3 toU8Vec3(const glm::vec3 &color);

private:
    size_t width{}, height{};
    size_t pixelCount{};
    std::unique_ptr<std::vector<float> > depthsPtr{};
    std::unique_ptr<std::vector<glm::u8vec3> > colorsPtr{};
};
