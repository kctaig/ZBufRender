#pragma once

#include <memory>
#include <vector>

#include "bbox.hpp"
#include "octree.hpp"
#include "shader.hpp"
#include "zbuffer.hpp"

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

    void checkFragMesh(const FragMesh& fragMesh, const Shader& shader, std::shared_ptr<ZBuffer> bufferPtr);

    void checkPixel(glm::ivec2 pixel, float pixelDepth, glm::vec3 color, std::shared_ptr<ZBuffer> bufferPtr);

    bool containFragMesh(const FragMesh& fragMesh) const;

    bool containPixel(glm::ivec2 pixel) const;

    bool containOctree(std::shared_ptr<Octree> octreeRoot) const;

    void checkOctree(std::shared_ptr<Octree> octreeRoot, const Shader& shader, std::shared_ptr<ZBuffer> bufferPtr);

    void updateDepth();

   private:
    float depth;
    std::shared_ptr<BBOX> bboxPtr;
    std::vector<std::shared_ptr<QuadTree>> children;
};