#include "buffer.hpp"

Buffer::Buffer(size_t width, size_t height)
    : width(width),
      height(height),
      pixelCount(width * height) {
    depthsPtr = std::make_unique<std::vector<float>>(pixelCount);
    colorsPtr = std::make_unique<std::vector<glm::u8vec3>>(pixelCount);
    clear();
}

void Buffer::clear(glm::vec3 color) {
    depthsPtr->assign(pixelCount, 1.f);
    colorsPtr->assign(pixelCount, Buffer::toU8Vec3(color));
}

size_t Buffer::getIndex(size_t x, size_t y) const {
    return y * width + x;
}

void Buffer::bufferResize(size_t w, size_t h, glm::vec3 color) {
    width = w;
    height = h;
    pixelCount = w * h;
    depthsPtr->resize(pixelCount, 1.f);
    colorsPtr->resize(pixelCount, toU8Vec3(color));
}

glm::u8vec3 Buffer::toU8Vec3(const glm::vec3 &color) {
    return glm::u8vec3{
        static_cast<unsigned char>(glm::clamp(color.x * 255.0f, 0.0f, 255.0f)),
        static_cast<unsigned char>(glm::clamp(color.y * 255.0f, 0.0f, 255.0f)),
        static_cast<unsigned char>(glm::clamp(color.z * 255.0f, 0.0f, 255.0f))
    };
}
