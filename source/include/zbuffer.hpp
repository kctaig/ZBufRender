#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "scanline_struct.hpp"

class ZBuffer {
public:
	ZBuffer() = default;

	virtual ~ZBuffer() = default;

	ZBuffer(size_t width, size_t height) : width(width), height(height) {
	}

	size_t getWidth() const { return width; }
	size_t getHeight() const { return height; }

	auto& getPixelPtr() { return pixelPtr; }
	auto& getDepthPtr() { return depthPtr; }

	glm::u8vec3 toU8Vec3(const glm::vec3& color);

	const auto getIndex(size_t x, size_t y) const { return y * width + x; };
	const auto getDepth(size_t x, size_t y) const { return (*depthPtr)[getIndex(x, y)]; }
	const auto& getColor(size_t x, size_t y) const { return (*pixelPtr)[getIndex(x, y)]; }

	void setDepth(size_t x, size_t y, float depth) { (*depthPtr)[getIndex(x, y)] = depth; }
	void setPixel(size_t x, size_t y, glm::vec3 color) { (*pixelPtr)[getIndex(x, y)] = toU8Vec3(color); }

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

	void clear(glm::vec3 color) override;

	void bufferResize(size_t w, size_t h, glm::vec3 color) override;

private:
	size_t pixelCount{};
};

class ScanLineZBuffer : public ZBuffer {
public:
	ScanLineZBuffer() = default;

	ScanLineZBuffer(size_t width, size_t height);

	auto& getCPTPtr() { return cptPtr; }
	auto& getAETPtr() { return aetPtr; }

	void fragMeshToCPT(const FragMesh& fragMesh, int id) const;

	void clear(glm::vec3 color) override;

	void clearDepth() { depthPtr->assign(width, 1.f); }

	auto getDepth(int x) { return depthPtr->at(x); }
	void setDepth(int x, float depth) { depthPtr->at(x) = depth; }

	void checkCPT(int y) const;
	void updateAET();

	void bufferResize(size_t w, size_t h, glm::vec3 color) override;

private:
	std::unique_ptr<std::vector<std::vector<CPTNode> > > cptPtr{}; // 分类多边形表指针
	std::unique_ptr<std::vector<AETNode> > aetPtr{};
};

