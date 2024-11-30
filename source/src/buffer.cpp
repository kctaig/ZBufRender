#include "buffer.hpp"

glm::u8vec3 ZBuffer::toU8Vec3(const glm::vec3 &color) {
    return glm::u8vec3{
        static_cast<unsigned char>(glm::clamp(color.x * 255.0f, 0.0f, 255.0f)),
        static_cast<unsigned char>(glm::clamp(color.y * 255.0f, 0.0f, 255.0f)),
        static_cast<unsigned char>(glm::clamp(color.z * 255.0f, 0.0f, 255.0f))
    };
}


RegularZBuffer::RegularZBuffer(size_t width, size_t height): ZBuffer(width, height), pixelCount(width * height) {
    depthPtr = std::make_unique<std::vector<float> >(pixelCount, 1.f);
    pixelPtr = std::make_unique<std::vector<glm::u8vec3> >(pixelCount, glm::u8vec3{0, 0, 0});
}

void RegularZBuffer::clear(glm::vec3 color) {
    depthPtr->assign(pixelCount, 1.f);
    pixelPtr->assign(pixelCount, toU8Vec3(color));
}

void RegularZBuffer::bufferResize(size_t w, size_t h, glm::vec3 color) {
    width = w;
    height = h;
    pixelCount = w * h;
    depthPtr->resize(pixelCount, 1.f);
    pixelPtr->resize(pixelCount, toU8Vec3(color));
}
