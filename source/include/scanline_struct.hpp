#pragma once

#include <iostream>
#include <vector>

struct CETNode {
    int x, dy;
    float dx;
    float z;

	CETNode(glm::vec4 v1, glm::vec4 v2);
};

struct CPTNode {
    float a, b, c, d;
    int id, dy;
    std::shared_ptr<std::vector<CETNode> > cetPtr{};
    glm::vec3 color;
};

struct AETNode {
    // left edge
    float xl;
    float dxl;  // -1/k
    int dyl;    // rest scan line number

    // right edge
    float xr;
    float dxr;
    int dyr;

    float zl;
    float dzx;  // dzx = - a / c
    float dzy;  // dzy = b / c
    std::shared_ptr<CPTNode> cptNodePtr;
};

inline CETNode::CETNode(glm::vec4 v1, glm::vec4 v2) {
    x = static_cast<int>(v2.x);
    z = v2.z;
    dy = static_cast<int>(v2.y) - static_cast<int>(v1.y);
    dx = -(v2.x - v1.x) / static_cast<float>(dy);
}