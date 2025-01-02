#pragma once

#include <glm/glm.hpp>
#include <vector>

struct BBOX {
    int minX, minY, maxX, maxY;

    BBOX() = default;

    ~BBOX() = default;

    BBOX(int minX, int minY, int maxX, int maxY)
        : minX(minX), minY(minY), maxX(maxX), maxY(maxY) {
    }

    BBOX(const std::vector<glm::vec4>& screenVertices);

    void limitedToBBox(const BBOX& screenBBox);

    bool containBBox(const BBOX& bbox) const;
};

struct BBOX3d : public BBOX {
    float minZ, maxZ;

    BBOX3d() = default;

    ~BBOX3d() = default;

    BBOX3d(int minX, int minY, int maxX, int maxY, float minZ, float maxZ)
        : BBOX(minX, minY, maxX, maxY), minZ(minZ), maxZ(maxZ) {
    }

    BBOX3d(BBOX bbox)
        : BBOX(bbox), minZ(0.f), maxZ(1.f) {}

    BBOX3d(const std::vector<glm::vec4>& screenVertices);

    bool containBBox3d(const BBOX3d& subBBox3d) const;
};