#pragma once

#include <memory>
#include "frag_mesh.hpp"

class Octree {
public:
	Octree() = default;
	~Octree() = default;

	Octree(const BBOX3d& bbox, const std::vector<std::shared_ptr<FragMesh>>& fragMeshesPtr, int level);

	void updateOctreeDepth();

	float getDepth() const { return depth; }
	std::vector<std::shared_ptr<FragMesh>> getFragMeshesPtr() const { return fragMeshesPtr; }
	std::vector<std::shared_ptr<Octree>> getChildren() const { return children; }
	std::shared_ptr<BBOX3d> getBBoxPtr() const { return bboxPtr; }

private:
	float depth;
	std::shared_ptr<BBOX3d> bboxPtr;
	std::vector<std::shared_ptr<FragMesh>> fragMeshesPtr;
	std::vector<std::shared_ptr<Octree>> children;
};