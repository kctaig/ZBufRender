#pragma once
#include <shader.hpp>

class BBOX {
public:
    BBOX() = default;

    ~BBOX() = default;

    BBOX(int minX, int minY, int maxX, int maxY)
        : minX(minX), minY(minY), maxX(maxX), maxY(maxY) {
    }

    void updateBBox(const FragMesh &fragMesh);

    int getMinX() const { return minX; }
    int getMinY() const { return minY; }
    int getMaxX() const { return maxX; }
    int getMaxY() const { return maxY; }

private:
    int minX, minY, maxX, maxY;
};
