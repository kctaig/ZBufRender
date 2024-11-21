#include <bbox.hpp>

void BBOX::updateBBox(const FragMesh& fragMesh) {
    for (size_t i = 0; i < fragMesh.vertexNum; i++) {
        auto vertex = fragMesh.screenMesh[i];
        minX = std::min(minX, static_cast<int>(vertex.x));
        minY = std::min(minY, static_cast<int>(vertex.y));
        maxX = std::max(maxX, static_cast<int>(vertex.x));
        maxY = std::max(maxY, static_cast<int>(vertex.y));
    }
}