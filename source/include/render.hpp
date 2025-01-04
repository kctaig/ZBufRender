#pragma once

#include <iostream>
#include <chrono>

#include "zbuffer.hpp"
#include "model.hpp"
#include "quad_tree.hpp"
#include "shader.hpp"
#include "octree.hpp"

enum RasterType {
	REGULAR,
	SCANLINE,
	NAIVE,
	OCTREE
};

class Render {
public:
	Render() = default;
	~Render() = default;

	Render(std::unique_ptr<Model> mPtr, std::shared_ptr<Camera> cPtr, std::shared_ptr<ZBuffer> fbPtr, RasterType type);

	void initFragMeshesPtr(const Uniforms& uniforms, const Shader& shader);

	void regularRender(const Uniforms& uniforms,
		const Shader& shader) const;

	void scanLineRender(const Shader& shader,
		const Uniforms& uniforms) const;

	void naiveHierarchyRender(const Shader& shader,
		const Uniforms& uniforms) const;

	void octreeHierarchyRender(const Shader& shader,
		const Uniforms& uniforms) const;

	auto getCameraPtr() const { return cameraPtr; }
	auto getBufferPtr() const { return bufferPtr; }
	auto getModelPtr() const { return modelPtr.get(); }
	auto getRasterType() const { return rasterType; }

	auto& getFragMeshesPtr() { return fragMeshesPtr; }
	auto getFragMeshesPtr() const { return fragMeshesPtr; }

private:
	std::unique_ptr<Model> modelPtr;
	std::vector<std::shared_ptr<FragMesh>> fragMeshesPtr = {};
	std::shared_ptr<Camera> cameraPtr;
	std::shared_ptr<ZBuffer> bufferPtr;
	RasterType rasterType;
};