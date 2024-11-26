#include <frame_buffer.hpp>

FrameBuffer::FrameBuffer(size_t width, size_t height)
    : width(width),
      height(height),
      pixelCount(width * height) {
    depthBuffer = std::make_unique<std::vector<float>>(pixelCount,1.f);
    colorBuffer = std::make_unique<std::vector<glm::vec3>>(pixelCount);
	screenBuffer = std::make_unique<std::vector<glm::u8vec3>>(pixelCount);
    clear(glm::vec3(1,0,0));
}

void FrameBuffer::clear(const glm::vec3 &color) {
	(*depthBuffer).assign(pixelCount, 1.f);
    (*colorBuffer).assign(pixelCount, color);
}

size_t FrameBuffer::getIndex(const size_t x, const size_t y) const {
    return y * width + x;
}

void FrameBuffer::reCreate(size_t w, size_t h,const glm::vec3 & color)
{
	width = w;
	height = h;
	pixelCount = w * h;
	depthBuffer->resize(pixelCount,1.f);
	colorBuffer->resize(pixelCount);
	screenBuffer->resize(pixelCount);
	clear(color);
}

const std::unique_ptr<std::vector<glm::u8vec3>>& FrameBuffer::getScreenBuffer()
{
    for (size_t i = 0; i < pixelCount;i++) {
		glm::vec3 color = (*colorBuffer)[i];
		glm::u8vec3 color_u8 = static_cast<glm::u8vec3>(color * 255.f);
        (*screenBuffer)[i] = color_u8;
    }
	return screenBuffer;
}
