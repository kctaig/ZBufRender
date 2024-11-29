#include "bbox.hpp"

void BBOX::updateBBox(const FragMesh &fragMesh) {
    int minx = static_cast<int>(floor(fragMesh.screenMesh[0].x));
    int miny = static_cast<int>(floor(fragMesh.screenMesh[0].y));
    int maxx = static_cast<int>(ceil(fragMesh.screenMesh[1].x));
    int maxy = static_cast<int>(ceil(fragMesh.screenMesh[1].y));

    for (size_t i = 1; i < fragMesh.vertexNum; i++) {
        const auto &vertex = fragMesh.screenMesh[i];
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
