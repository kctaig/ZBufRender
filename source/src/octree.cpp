#include "octree.hpp"
#include <numeric>

Octree::Octree(const BBOX3d& bbox, const std::vector<std::shared_ptr<FragMesh>>& fragMeshesPtr, int level) {
	bboxPtr = std::make_shared<BBOX3d>(bbox);
	depth = bbox.minZ;

	if (fragMeshesPtr.size() <= 1 || bbox.minX + 1 >= bbox.maxX || bbox.minY + 1 >= bbox.maxY || level >= 8) {
		this->fragMeshesPtr = fragMeshesPtr;
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

	std::vector<std::vector<std::shared_ptr<FragMesh>>> childrenFragMeshes(8);
	for (const auto& fragMeshPtr : fragMeshesPtr) {
		auto fragMeshBBox = fragMeshPtr->bbox;
		if (leftDownFrontBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[0].push_back(fragMeshPtr);
		else if (rightDownFrontBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[1].push_back(fragMeshPtr);
		else if (leftUpFrontBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[2].push_back(fragMeshPtr);
		else if (rightUpFrontBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[3].push_back(fragMeshPtr);
		else if (leftDownBackBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[4].push_back(fragMeshPtr);
		else if (rightDownBackBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[5].push_back(fragMeshPtr);
		else if (leftUpBackBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[6].push_back(fragMeshPtr);
		else if (rightUpBackBBox.containBBox3d(fragMeshBBox)) childrenFragMeshes[7].push_back(fragMeshPtr);
		else this->fragMeshesPtr.push_back(fragMeshPtr);
	}

	if (!childrenFragMeshes[0].empty())
		children.push_back(std::make_shared<Octree>(leftDownFrontBBox, childrenFragMeshes[0], level + 1));
	if (!childrenFragMeshes[1].empty())
		children.push_back(std::make_shared<Octree>(rightDownFrontBBox, childrenFragMeshes[1], level + 1));
	if (!childrenFragMeshes[2].empty())
		children.push_back(std::make_shared<Octree>(leftUpFrontBBox, childrenFragMeshes[2], level + 1));
	if (!childrenFragMeshes[3].empty())
		children.push_back(std::make_shared<Octree>(rightUpFrontBBox, childrenFragMeshes[3], level + 1));
	if (!childrenFragMeshes[4].empty())
		children.push_back(std::make_shared<Octree>(leftDownBackBBox, childrenFragMeshes[4], level + 1));
	if (!childrenFragMeshes[5].empty())
		children.push_back(std::make_shared<Octree>(rightDownBackBBox, childrenFragMeshes[5], level + 1));
	if (!childrenFragMeshes[6].empty())
		children.push_back(std::make_shared<Octree>(leftUpBackBBox, childrenFragMeshes[6], level + 1));
	if (!childrenFragMeshes[7].empty())
		children.push_back(std::make_shared<Octree>(rightUpBackBBox, childrenFragMeshes[7], level + 1));

	//updateOctreeDepth();
}

void Octree::updateOctreeDepth() {
	if (children.empty())
		return;
	depth = std::accumulate(children.begin(), children.end(), FLT_MAX,
		[](float acc, const std::shared_ptr<Octree>& child) {
			return std::min(acc, child->depth);
		});
}