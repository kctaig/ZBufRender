#pragma once

#include <memory>
#include <vector>
#include "bbox.hpp"

class QuadTree {
   public:
    QuadTree() = default;
    ~QuadTree() = default;

    QuadTree(BBOX& bbox);

    std::shared_ptr<QuadTree> getChildren(int index) const { return children[index]; }
    std::shared_ptr<BBOX> getBBoxPtr() const { return bboxPtr; }

    void setDepth(float d) { depth = d; }
    void resetDepth(float d);
    float getDepth() const { return depth; }

    void checkFragMesh(const FragMesh& fragMesh);

    bool containFragMesh(const FragMesh& fragMesh) const;

    void updateDepth();

    float calculateDepth(glm::ivec2 pixel,
                                   const FragMesh& fragMesh) const;

    glm::vec3 calculateWeights(const FragMesh& fragMesh,
                                         const glm::vec2& screenPoint);

   private:
    float depth;
    std::shared_ptr<BBOX> bboxPtr;
    std::vector<std::shared_ptr<QuadTree>> children;
};