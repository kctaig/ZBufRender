#include <bbox.hpp>

void BBOX::updateBBox(const FragMesh& fragMesh) {
	int minx = fragMesh.screenMesh[0].x;
    int miny = fragMesh.screenMesh[0].y;
    int maxx = fragMesh.screenMesh[0].x;
    int maxy = fragMesh.screenMesh[0].y;

    for (size_t i = 1; i < fragMesh.vertexNum; i++) {
        auto vertex = fragMesh.screenMesh[i];
        minx = std::min(minx, static_cast<int>(vertex.x));
        miny = std::min(miny, static_cast<int>(vertex.y));
        maxx = std::max(maxx, static_cast<int>(vertex.x));
        maxy = std::max(maxy, static_cast<int>(vertex.y));
    }

	minX = std::max(minX, minx);
	minY = std::max(minY, miny);
	maxX = std::min(maxX, maxx);
	maxY = std::min(maxY, maxy);

    minX = std::floor(minX);
    minY = std::floor(minY);
    maxX = std::ceil(maxX);
    maxY = std::ceil(maxY);
}