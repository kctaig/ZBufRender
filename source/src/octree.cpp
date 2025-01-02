#include "octree.hpp"

Octree::Octree(const BBOX3d& bbox, const std::vector<FragMesh>& fragMeshes) {
	int midX = (bbox.minX + bbox.maxX) / 2;
	int midY = (bbox.minY + bbox.maxY) / 2;

	if (fragMeshes.size() < 1 || bbox.minX <= midX || bbox.minY <= midY) {
		for (const auto& fragMesh : fragMeshes) {
			fragMeshesPtr.push_back(std::make_shared<FragMesh>(fragMesh));
		}
		return;
	}

	float midZ = (bbox.minZ + bbox.maxZ) / 2;

	bboxPtr = std::make_shared<BBOX3d>(bbox);
	depth = bbox.minZ;

	std::vector<std::vector<FragMesh>> childrenFragMeshes(8);
	for (const auto& fragMesh : fragMeshes) {
		auto fragMeshBBox = fragMesh.bbox;
		if (fragMeshBBox.minX < midX && fragMeshBBox.minY < midY && fragMeshBBox.minZ < midZ) {  // left down front
			childrenFragMeshes[0].push_back(fragMesh);
		}
		else if (fragMeshBBox.minX >= midX && fragMeshBBox.minY < midY && fragMeshBBox.minZ < midZ) {  // right down front
			childrenFragMeshes[1].push_back(fragMesh);
		}
		else if (fragMeshBBox.minX < midX && fragMeshBBox.minY >= midY && fragMeshBBox.minZ < midZ) {  // left up front
			childrenFragMeshes[2].push_back(fragMesh);
		}
		else if (fragMeshBBox.minX >= midX && fragMeshBBox.minY >= midY && fragMeshBBox.minZ < midZ) {  // right up front
			childrenFragMeshes[3].push_back(fragMesh);
		}
		else if (fragMeshBBox.minX < midX && fragMeshBBox.minY < midY && fragMeshBBox.minZ >= midZ) {  // left down back
			childrenFragMeshes[4].push_back(fragMesh);
		}
		else if (fragMeshBBox.minX >= midX && fragMeshBBox.minY < midY && fragMeshBBox.minZ >= midZ) {  // right down back
			childrenFragMeshes[5].push_back(fragMesh);
		}
		else if (fragMeshBBox.minX < midX && fragMeshBBox.minY >= midY && fragMeshBBox.minZ >= midZ) {  // left up back
			childrenFragMeshes[6].push_back(fragMesh);
		}
		else if (fragMeshBBox.minX >= midX && fragMeshBBox.minY >= midY && fragMeshBBox.minZ >= midZ) {  // right up back
			childrenFragMeshes[7].push_back(fragMesh);
		}
		else {
			fragMeshesPtr.push_back(std::make_shared<FragMesh>(fragMesh));
		}
	}

	children.push_back(std::make_shared<Octree>(BBOX3d{ bbox.minX, bbox.minY, bbox.minZ, midX, midY, midZ }, childrenFragMeshes[0]));
	children.push_back(std::make_shared<Octree>(BBOX3d{ midX, bbox.minY, bbox.minZ, bbox.maxX, midY, midZ }, childrenFragMeshes[1]));
	children.push_back(std::make_shared<Octree>(BBOX3d{ bbox.minX, midY, bbox.minZ, midX, bbox.maxY, midZ }, childrenFragMeshes[2]));
	children.push_back(std::make_shared<Octree>(BBOX3d{ midX, midY, bbox.minZ, bbox.maxX, bbox.maxY, midZ }, childrenFragMeshes[3]));
	children.push_back(std::make_shared<Octree>(BBOX3d{ bbox.minX, bbox.minY, midZ, midX, midY, bbox.maxZ }, childrenFragMeshes[4]));
	children.push_back(std::make_shared<Octree>(BBOX3d{ midX, bbox.minY, midZ, bbox.maxX, midY, bbox.maxZ }, childrenFragMeshes[5]));
	children.push_back(std::make_shared<Octree>(BBOX3d{ bbox.minX, midY, midZ, midX, bbox.maxY, bbox.maxZ }, childrenFragMeshes[6]));
	children.push_back(std::make_shared<Octree>(BBOX3d{ midX, midY, midZ, bbox.maxX, bbox.maxY, bbox.maxZ }, childrenFragMeshes[7]));

	updateDepth();
}

void Octree::updateDepth() {
	if (children.empty())
		return;

	float tempDepth = FLT_MAX;
	for (auto& child : children) {
		tempDepth = std::min(tempDepth, child->depth);
	}
	depth = tempDepth;
}