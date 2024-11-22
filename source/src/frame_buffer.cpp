#include <frame_buffer.hpp>

FrameBuffer::FrameBuffer(size_t width, size_t height)
    : width_(width),
      height_(height),
      pixelCount_(width * height) {
    depthBuffer_ = std::make_unique<float[]>(pixelCount_);
    colorBuffer_ = std::make_unique<std::vector<glm::vec3>>(pixelCount_);
	screenBuffer_ = std::make_shared<std::vector<glm::u8vec3>>(pixelCount_);
    clear(glm::vec3(0));
}

void FrameBuffer::clear(const glm::vec3 &color) {
    std::fill(depthBuffer_.get(), depthBuffer_.get() + pixelCount_, 1.0f);
    (*colorBuffer_).assign(pixelCount_, color);
}

size_t FrameBuffer::getIndex(const size_t x, const size_t y) const {
    try {
        if (x >= width_ || y >= height_) {
            throw std::out_of_range("FrameBuffer x or y out of range");
        }
    } catch (std::out_of_range& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    return y * width_ + x;
}

void FrameBuffer::reCreate(size_t width, size_t height,const glm::vec3 & color)
{
	width_ = width;
	height_ = height;
	pixelCount_ = width * height;
	depthBuffer_ = std::make_unique<float[]>(pixelCount_);
	colorBuffer_->resize(pixelCount_);
	screenBuffer_->resize(pixelCount_);
	clear(color);
}

const std::shared_ptr<std::vector<glm::u8vec3>>& FrameBuffer::getScreenBuffer()
{
    for (size_t i = 0; i < pixelCount_;i++) {
		glm::vec3 color = (*colorBuffer_)[i];
		glm::u8vec3 color_u8 = static_cast<glm::u8vec3>(color * 255.f);
        (*screenBuffer_)[i] = color_u8;
    }
	return screenBuffer_;

}
