#pragma once

#include <glm/glm.hpp>
#include <memory>

class FrameBuffer {
public:
    FrameBuffer() = default;

    ~FrameBuffer() = default;

    FrameBuffer(size_t width, size_t height);

    void clear(glm::vec3 color = {0, 0, 0});

    size_t getWidth() const { return width; }
    size_t getHeight() const { return height; }

    size_t getIndex(size_t x, size_t y) const;

    float getDepth(size_t x, size_t y) const { return (*depthBuffer)[getIndex(x, y)]; }
    const glm::vec3 &getColor(size_t x, size_t y) const { return (*colorBuffer)[getIndex(x, y)]; }
    void setDepth(size_t x, size_t y, float depth) { (*depthBuffer)[getIndex(x, y)] = depth; }
    void setPixel(size_t x, size_t y, glm::vec3 color) { (*colorBuffer)[getIndex(x, y)] = color; }

    void reCreate(size_t w, size_t h, glm::vec3 color = {0, 0, 0});

    const std::unique_ptr<std::vector<glm::u8vec3> > &getScreenBuffer();

private:
    size_t width{}, height{};
    size_t pixelCount{};
    std::unique_ptr<std::vector<float> > depthBuffer;
    std::unique_ptr<std::vector<glm::vec3> > colorBuffer;
    std::unique_ptr<std::vector<glm::u8vec3> > screenBuffer;
};
