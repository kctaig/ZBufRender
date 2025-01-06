#pragma once

#include <vector>
#include <memory>
#include "frag_mesh.hpp"

class KDTree {
public:
	KDTree() = default;
	~KDTree() = default;

	KDTree(const BBOX3d& bbox, const std::vector<std::shared_ptr<FragMesh>>& fragMeshPtr, int split);

	void updateKDTreeDepth();

	float getDepth() const { return depth; }
	std::shared_ptr<BBOX3d> getBBoxPtr() const { return bboxPtr; }
	std::shared_ptr<BBOX3d>& getBBoxPtr() { return bboxPtr; }
	std::vector<std::shared_ptr<FragMesh>> getFragMeshesPtr() const { return fragMeshPtr; }
	std::vector<std::shared_ptr<FragMesh>>& getFragMeshesPtr() { return fragMeshPtr; }
	std::shared_ptr<KDTree> getLeftChild() const { return leftChild; }
	std::shared_ptr<KDTree> getRightChild() const { return rightChild; }

private:
	float depth;
	std::shared_ptr<BBOX3d>bboxPtr;
	std::vector<std::shared_ptr<FragMesh>> fragMeshPtr;
	std::shared_ptr<KDTree> leftChild;
	std::shared_ptr<KDTree> rightChild;
};