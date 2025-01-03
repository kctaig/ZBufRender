#pragma once

#include <memory>
#include "frag_mesh.hpp"
class Octree {
public:
	Octree() = default;

	~Octree() = default;

	Octree(const BBOX3d& bbox, const std::vector<std::shared_ptr<FragMesh>>& fragMeshesPtr);
	void updateOctreeDepth();

	float getDepth() const { return depth; }
	auto getFragMeshesPtr() const { return fragMeshesPtr; }
	auto getChildren() const { return children; }
	auto getBBoxPtr() const { return bboxPtr; }

private:
	float depth;
	std::shared_ptr<BBOX3d> bboxPtr;
	std::vector<std::shared_ptr<FragMesh>> fragMeshesPtr;
	std::vector<std::shared_ptr<Octree>> children;
};