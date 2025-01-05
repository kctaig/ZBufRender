#include "kd_tree.hpp"

KDTree::KDTree(const BBOX3d& bbox, const std::vector<std::shared_ptr<FragMesh>>& fragMeshPtr, int split)
{
	// update the current node
	this->bboxPtr = std::make_shared<BBOX3d>(bbox);
	this->depth = bbox.minZ;

	if (fragMeshPtr.size() <= 1 || split >= 8) {
		this->fragMeshPtr = fragMeshPtr;
		return;
	}

	// Split the bounding box
	BBOX3d leftBbox = bbox;
	BBOX3d rightBbox = bbox;
	if (split % 3 == 0) {
		int mid = (bbox.minX + bbox.maxX) / 2;
		leftBbox.maxX = mid;
		rightBbox.minX = mid;
	}
	else if (split % 3 == 1) {
		int mid = (bbox.minY + bbox.maxY) / 2;
		leftBbox.maxY = mid;
		rightBbox.minY = mid;
	}
	else if (split % 3 == 2) {
		float mid = (bbox.minZ + bbox.maxZ) / 2;
		leftBbox.maxZ = mid;
		rightBbox.minZ = mid;
	}

	// Split the fragment mesh
	std::vector<std::shared_ptr<FragMesh>> leftFragMeshPtr;
	std::vector<std::shared_ptr<FragMesh>> rightFragMeshPtr;
	for (auto& fragMeshPtr : fragMeshPtr) {
		if (leftBbox.containBBox3d(fragMeshPtr->bbox)) {
			leftFragMeshPtr.push_back(fragMeshPtr);
		}
		else if (rightBbox.containBBox3d(fragMeshPtr->bbox)) {
			rightFragMeshPtr.push_back(fragMeshPtr);
		}
		else {
			this->fragMeshPtr.push_back(fragMeshPtr);
		}
	}

	//Create left and right child
	if (!leftFragMeshPtr.empty()) {
		leftChild = std::make_shared<KDTree>(leftBbox, leftFragMeshPtr, split + 1);
	}
	if (!rightFragMeshPtr.empty()) {
		rightChild = std::make_shared<KDTree>(rightBbox, rightFragMeshPtr, split + 1);
	}

	updateKDTreeDepth();
}

void KDTree::updateKDTreeDepth()
{
	if (leftChild) {
		depth = std::min(depth, leftChild->depth);
	}
	if (rightChild) {
		depth = std::min(depth, rightChild->depth);
	}
}