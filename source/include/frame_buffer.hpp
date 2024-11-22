#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <memory>

class FrameBuffer
{

public:
    FrameBuffer(size_t width, size_t height);
    ~FrameBuffer() = default;

    void clear(const glm::vec3 &color = {1,1,1});

    size_t getIndex(const size_t x, const size_t y) const;

    void setPixel(const size_t x, const size_t y, const glm::vec3& color) { (*colorBuffer_)[getIndex(x, y)] = color; }
    void setPixel(const size_t index, const glm::vec3 &color) { (*colorBuffer_)[index] = color; }

    size_t getWidth() const { return width_; }
    size_t getHeight() const { return height_; }

    float getDepth(const size_t x, const size_t y) const { return depthBuffer_.get()[getIndex(x, y)]; }
    float getDepth(const size_t index) const { return depthBuffer_.get()[index]; }

    glm::vec3 getColor(const size_t x, const size_t y) const { return (*colorBuffer_)[getIndex(x, y)]; }
    glm::vec3 getColor(const size_t index) const { return (*colorBuffer_)[index]; }

    //void writePPM(const char *filename) const;

	void reCreate(size_t width, size_t height, const glm::vec3 &color = {0,0,0});

	std::unique_ptr<std::vector<glm::vec3>>& getColorBuffer() { return colorBuffer_; }

    const std::shared_ptr<std::vector<glm::u8vec3>>& getScreenBuffer();

private:
    size_t width_, height_;
    size_t pixelCount_;
    std::unique_ptr<float[]> depthBuffer_;
    std::unique_ptr<std::vector<glm::vec3>> colorBuffer_;
	std::shared_ptr<std::vector<glm::u8vec3>> screenBuffer_;
};