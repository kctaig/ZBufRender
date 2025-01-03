#include "bbox.hpp"

BBOX::BBOX(const std::vector<glm::vec4>& screenVertices) {
	minX = std::min(screenVertices[0].x, std::min(screenVertices[1].x, screenVertices[2].x));
	maxX = std::max(screenVertices[0].x, std::max(screenVertices[1].x, screenVertices[2].x));
	minY = std::min(screenVertices[0].y, std::min(screenVertices[1].y, screenVertices[2].y));
	maxY = std::max(screenVertices[0].y, std::max(screenVertices[1].y, screenVertices[2].y));
}

// 限定于 screenBBoxBBox 内
void BBOX::limitedToBBox(const BBOX& screenBBox) {
	minX = std::max(minX, screenBBox.minX);
	minY = std::max(minY, screenBBox.minY);
	maxX = std::min(maxX, screenBBox.maxX);
	maxY = std::min(maxY, screenBBox.maxY);
}

bool BBOX::containBBox(const BBOX& subBBox) const {
	return subBBox.minX >= minX && subBBox.minY >= minY && subBBox.maxX <= maxX && subBBox.maxY <= maxY;
}

/***************************************** 3d BBox ******************************************/

BBOX3d::BBOX3d(const std::vector<glm::vec4>& screenVertices)
	: BBOX(screenVertices) {
	minZ = std::min(screenVertices[0].z, std::min(screenVertices[1].z, screenVertices[2].z));
	maxZ = std::max(screenVertices[0].z, std::max(screenVertices[1].z, screenVertices[2].z));
}

bool BBOX3d::containBBox3d(const BBOX3d& subBBox3d) const {
	return containBBox(subBBox3d) && subBBox3d.minZ >= minZ && subBBox3d.maxZ <= maxZ;
}