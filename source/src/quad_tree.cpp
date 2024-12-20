#include "quad_tree.hpp"
#include <bits/algorithmfwd.h>

QuadTree::QuadTree(BBOX& bbox) {
    depth = 1.f;

    int minX = bbox.getMinX();
    int minY = bbox.getMinY();
    int maxX = bbox.getMaxX();
    int maxY = bbox.getMaxY();

    int midX = (minX + maxX) / 2;
    int midY = (minY + maxY) / 2;

    if (minX + 1 == maxX && minY + 1 == maxY) {
        return;
    } else if (minX + 1 < maxX && minY + 1 == maxY) {
        children.push_back(std::make_shared<QuadTree>(BBOX(minX, minY, midX, maxY)));
        children.push_back(std::make_shared<QuadTree>(BBOX(midX, minY, maxX, maxY)));
    } else if (minX + 1 == maxX && minY + 1 < maxY) {
        children.push_back(std::make_shared<QuadTree>(BBOX(minX, minY, maxX, midY)));
        children.push_back(std::make_shared<QuadTree>(BBOX(minX, midY, maxX, maxY)));
    } else {
        children.push_back(std::make_shared<QuadTree>(BBOX(minX, minY, midX, midY)));
        children.push_back(std::make_shared<QuadTree>(BBOX(midX, minY, maxX, midY)));
        children.push_back(std::make_shared<QuadTree>(BBOX(minX, midY, midX, maxY)));
        children.push_back(std::make_shared<QuadTree>(BBOX(midX, midY, maxX, maxY)));
    }
}

void QuadTree::resetDepth(float d) {
    depth = 1.f;
    for (auto& child : children) {
        child->resetDepth(d);
    }
}

void QuadTree::updateDepth() {
    float tempDepth = 0.f;
    for (auto& child : children) {
        tempDepth = std::max(tempDepth, child->getDepth());
    }
    depth = tempDepth;
}

bool QuadTree::containFragMesh(const FragMesh& fragMesh) const {
    return bboxPtr->containFragMesh(fragMesh);
}

void QuadTree::checkFragMesh(const FragMesh& fragMesh) {
    // fragMesh已被消隐，无需被绘制
    if (fragMesh.zmin > depth)
        return;

    // 判断子节点是否包含fragMesh
    bool flag = false;
    for (auto& child : children) {
        if (child->containFragMesh(fragMesh)) {
            child->checkFragMesh(fragMesh);
            flag = true;
            break;
        }
    }
    if (!flag) {
        // 绘制fragMesh，更新深度
        vector<glm::ivec2> pixels;
        for (int x = bboxPtr->getMinX(); x < bboxPtr->getMaxX(); x++) {
            for (int y = bboxPtr->getMinY(); y < bboxPtr->getMaxY(); y++) {
                auto depth = calculateDepth({x, y}, fragMesh);
            }
        }
    }
    // 更新当前节点的深度
    updateDepth();
}

float QuadTree::calculateDepth(glm::ivec2 pixel,
                               const FragMesh& fragMesh) const {
    const int x = pixel.x;
    const int y = pixel.y;
    const glm::vec2 screenPoint(static_cast<float>(x) + .5f, static_cast<float>(y) + .5f);
    glm::vec3 weights = calculateWeights(fragMesh, screenPoint);
    // 检查重心坐标是否无效
    if (std::any_of(&weights[0], &weights[0] + 3,
                    [](const float w) { return w < -1e-5; })) {
        return INT_MAX;
    }
    // depth test
    float depth = fragMesh.v2d[0].z * weights[0] +
                  fragMesh.v2d[1].z * weights[1] +
                  fragMesh.v2d[2].z * weights[2];

    return depth;
}

glm::vec3 QuadTree::calculateWeights(const FragMesh& fragMesh,
                                     const glm::vec2& screenPoint) {
    glm::vec3 weights(0.f), screenWeights(0.f);
    const glm::vec4 fragCoords[3] = {
        fragMesh.v2d[0],
        fragMesh.v2d[1],
        fragMesh.v2d[2]};

    // 计算边向量和屏幕点向量
    const glm::vec2 ab = fragCoords[1] - fragCoords[0];
    const glm::vec2 ac = fragCoords[2] - fragCoords[0];
    const glm::vec2 ap = screenPoint - glm::vec2(fragCoords[0]);

    // 计算行列式的值，用于计算重心坐标
    const float det = ab.x * ac.y - ab.y * ac.x;

    if (std::abs(det) < 1e-5) {
        return glm::vec3(-1e-5 - 1);  // 如果行列式太小，认为点不在三角形内
    }

    // 计算重心坐标
    const float factor = 1.0f / det;
    const float s = (ac.y * ap.x - ac.x * ap.y) * factor;
    const float t = (ab.x * ap.y - ab.y * ap.x) * factor;

    screenWeights[0] = 1.0f - s - t;
    screenWeights[1] = s;
    screenWeights[2] = t;

    // 如果重心坐标无效（小于0或者大于1），则跳过该点
    if (s < -1e-5 || t < -1e-5 || s + t > 1.0f + 1e-5) {
        return glm::vec3(-1e-5 - 1);  // 该点不在三角形内
    }

    // 计算每个顶点的 w 值
    const float w0 = fragCoords[0].w * screenWeights[0];
    const float w1 = fragCoords[1].w * screenWeights[1];
    const float w2 = fragCoords[2].w * screenWeights[2];

    // 归一化重心坐标
    const float normalizer = 1.0f / (w0 + w1 + w2);
    weights[0] = w0 * normalizer;
    weights[1] = w1 * normalizer;
    weights[2] = w2 * normalizer;

    return weights;
}
