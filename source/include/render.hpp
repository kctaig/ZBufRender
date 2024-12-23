#pragma once

#include <omp.h>
#include <iostream>

#include "zbuffer.hpp"
#include "model.hpp"
#include "quad_tree.hpp"
#include "shader.hpp"

enum RasterType {
	REGULAR,
	SCANLINE,
	NAIVE
};

class Render {
public:
	Render() = default;

	~Render() = default;
	Render(std::unique_ptr<Model> mPtr,
		std::shared_ptr<Camera> cPtr,
		std::shared_ptr<ZBuffer> fbPtr,
		RasterType type) {
		modelPtr = std::move(mPtr);
		cameraPtr = cPtr;
		bufferPtr = fbPtr;
		rasterType = type;
	}

	void regularRender(const Uniforms& uniforms,
		const Shader& shader,
		bool useParallel = false) const;

	void scanLineRender(const Shader& shader,
		const Uniforms& uniforms) const;

	void naiveHierarchyRender(const Shader& shader,
		const Uniforms& uniforms,
		bool useParallel = false) const;

	auto getCameraPtr() const { return cameraPtr; }
	auto getBufferPtr() const { return bufferPtr; }
	auto getModelPtr() const { return modelPtr.get(); }
	auto getRasterType() const { return rasterType; }

private:
	std::unique_ptr<Model> modelPtr;
	std::shared_ptr<Camera> cameraPtr;
	std::shared_ptr<ZBuffer> bufferPtr;
	RasterType rasterType;
};