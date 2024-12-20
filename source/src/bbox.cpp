#include "bbox.hpp"

void BBOX::updateBBox(const FragMesh& fragMesh) {
	int minx = static_cast<int>(floor(fragMesh.v2d[0].x));
	int miny = static_cast<int>(floor(fragMesh.v2d[0].y));
	int maxx = static_cast<int>(ceil(fragMesh.v2d[0].x));
	int maxy = static_cast<int>(ceil(fragMesh.v2d[0].y));

	for (size_t i = 1; i < fragMesh.vertexNum; i++) {
		const auto& vertex = fragMesh.v2d[i];
		minx = std::min(minx, static_cast<int>(vertex.x));
		miny = std::min(miny, static_cast<int>(vertex.y));
		maxx = std::max(maxx, static_cast<int>(vertex.x));
		maxy = std::max(maxy, static_cast<int>(vertex.y));
	}

	minX = std::max(minX, minx);
	minY = std::max(minY, miny);
	maxX = std::min(maxX, maxx);
	maxY = std::min(maxY, maxy);
}

bool BBOX::containFragMesh(const FragMesh& fragMesh) const {
    float meshMinX = std::min(fragMesh.v2d[0].x, std::min(fragMesh.v2d[1].x, fragMesh.v2d[2].x));
    float meshMinY = std::min(fragMesh.v2d[0].y, std::min(fragMesh.v2d[1].y, fragMesh.v2d[2].y));
    float meshMaxX = std::max(fragMesh.v2d[0].x, std::max(fragMesh.v2d[1].x, fragMesh.v2d[2].x));
    float meshMaxY = std::max(fragMesh.v2d[0].y, std::max(fragMesh.v2d[1].y, fragMesh.v2d[2].y));

    return meshMinX >= minX && meshMinY >= minY && meshMaxX <= maxX && meshMaxY <= maxY;
}