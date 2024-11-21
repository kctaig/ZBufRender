#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <memory>

class FrameBuffer
{

public:
    FrameBuffer(size_t width, size_t height);
    ~FrameBuffer() = default;

    void clear();

    size_t getIndex(const size_t x, const size_t y) const;

    void setPixel(const size_t x, const size_t y, const glm::vec3& color) { colorBuffer_.get()[getIndex(x, y)] = color; }
    void setPixel(const size_t index, const glm::vec3 &color) { colorBuffer_.get()[index] = color; }

    size_t getWidth() const { return width_; }
    size_t getHeight() const { return height_; }

    float getDepth(const size_t x, const size_t y) const { return depthBuffer_.get()[getIndex(x, y)]; }
    float getDepth(const size_t index) const { return depthBuffer_.get()[index]; }

    glm::vec3 getColor(const size_t x, const size_t y) const { return colorBuffer_.get()[getIndex(x, y)]; }
    glm::vec3 getColor(const size_t index) const { return colorBuffer_.get()[index]; }

    void writePPM(const char *filename) const;

private:
    size_t width_, height_;
    size_t pixelCount_;
    std::unique_ptr<float[]> depthBuffer_;
    std::unique_ptr<glm::vec3[]> colorBuffer_;
};