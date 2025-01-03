#include "octree.hpp"

Octree::Octree(const BBOX3d& bbox, const std::vector<FragMesh>& fragMeshes) {
	bboxPtr = std::make_shared<BBOX3d>(bbox);
	depth = bbox.minZ;

	if (bbox.minX + 1 >= bbox.maxX || bbox.minY + 1 >= bbox.maxY) {
		for (const auto& fragMesh : fragMeshes) {
			fragMeshesPtr.push_back(std::make_shared<FragMesh>(fragMesh));
		}
		return;
	}

	int midX = (bbox.minX + bbox.maxX) / 2;
	int midY = (bbox.minY + bbox.maxY) / 2;
	float midZ = (bbox.minZ + bbox.maxZ) / 2.f;

	BBOX3d leftDownFrontBBox{ bbox.minX, bbox.minY, bbox.minZ, midX, midY, midZ };
	BBOX3d rightDownFrontBBox{ midX, bbox.minY, bbox.minZ, bbox.maxX, midY, midZ };
	BBOX3d leftUpFrontBBox{ bbox.minX, midY, bbox.minZ, midX, bbox.maxY, midZ };
	BBOX3d rightUpFrontBBox{ midX, midY, bbox.minZ, bbox.maxX, bbox.maxY, midZ };
	BBOX3d leftDownBackBBox{ bbox.minX, bbox.minY, midZ, midX, midY, bbox.maxZ };
	BBOX3d rightDownBackBBox{ midX, bbox.minY, midZ, bbox.maxX, midY, bbox.maxZ };
	BBOX3d leftUpBackBBox{ bbox.minX, midY, midZ, midX, bbox.maxY, bbox.maxZ };
	BBOX3d rightUpBackBBox{ midX, midY, midZ, bbox.maxX, bbox.maxY, bbox.maxZ };

	std::vector<std::vector<FragMesh>> childrenFragMeshes(8);
	for (const auto& fragMesh : fragMeshes) {
		auto fragMeshBBox = fragMesh.bbox;
		if (leftDownFrontBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[0].push_back(fragMesh);
		else if (rightDownFrontBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[1].push_back(fragMesh);
		else if (leftUpFrontBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[2].push_back(fragMesh);
		else if (rightUpFrontBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[3].push_back(fragMesh);
		else if (leftDownBackBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[4].push_back(fragMesh);
		else if (rightDownBackBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[5].push_back(fragMesh);
		else if (leftUpBackBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[6].push_back(fragMesh);
		else if (rightUpBackBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[7].push_back(fragMesh);
		else fragMeshesPtr.push_back(std::make_shared<FragMesh>(fragMesh));
	}

	if (!childrenFragMeshes[0].empty())
		children.push_back(std::make_shared<Octree>(leftDownFrontBBox, childrenFragMeshes[0]));
	if (!childrenFragMeshes[1].empty())
		children.push_back(std::make_shared<Octree>(rightDownFrontBBox, childrenFragMeshes[1]));
	if (!childrenFragMeshes[2].empty())
		children.push_back(std::make_shared<Octree>(leftUpFrontBBox, childrenFragMeshes[2]));
	if (!childrenFragMeshes[3].empty())
		children.push_back(std::make_shared<Octree>(rightUpFrontBBox, childrenFragMeshes[3]));
	if (!childrenFragMeshes[4].empty())
		children.push_back(std::make_shared<Octree>(leftDownBackBBox, childrenFragMeshes[4]));
	if (!childrenFragMeshes[5].empty())
		children.push_back(std::make_shared<Octree>(rightDownBackBBox, childrenFragMeshes[5]));
	if (!childrenFragMeshes[6].empty())
		children.push_back(std::make_shared<Octree>(leftUpBackBBox, childrenFragMeshes[6]));
	if (!childrenFragMeshes[7].empty())
		children.push_back(std::make_shared<Octree>(rightUpBackBBox, childrenFragMeshes[7]));

	updateOctreeDepth();
}

void Octree::updateOctreeDepth() {
	if (children.empty())
		return;

	float tempDepth = FLT_MAX;
	for (auto& child : children) {
		tempDepth = std::min(tempDepth, child->depth);
	}
	depth = tempDepth;
}