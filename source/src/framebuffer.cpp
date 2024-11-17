#include <framebuffer.hpp>

FrameBuffer::FrameBuffer(size_t width, size_t height) : width_(width), height_(height), pixelCount_(width * height)
{
    depthBuffer_ = std::make_unique<float[]>(pixelCount_);
    colorBuffer_ = std::make_unique<glm::vec3[]>(pixelCount_);
    clear();
}

void FrameBuffer::clear()
{
    std::fill(depthBuffer_.get(), depthBuffer_.get() + pixelCount_, 1.0f);
    std::fill(colorBuffer_.get(), colorBuffer_.get() + pixelCount_, glm::vec3(0.0f));
}

size_t FrameBuffer::getIndex(const size_t x, const size_t y) const
{
    try
    {
        if (x >= width_ || y >= height_)
        {
            throw std::out_of_range("FrameBuffer x or y out of range");
        }
    }
    catch (std::out_of_range &e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    return y * width_ + x;
}