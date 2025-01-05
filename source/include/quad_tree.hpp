#pragma once

#include <memory>
#include <vector>

#include "bbox.hpp"
#include "octree.hpp"
#include "shader.hpp"
#include <kd_tree.hpp>

class ZBuffer;

class QuadTree {
public:
	QuadTree() = default;
	~QuadTree() = default;

	QuadTree(BBOX& bbox);

	std::shared_ptr<QuadTree> getChildren(int index) const { return children[index]; }
	std::shared_ptr<BBOX> getBBoxPtr() const { return bboxPtr; }

	void setDepth(float d) { depth = d; }
	void resetDepth(float d = FLT_MAX);
	float getDepth() const { return depth; }

	bool containPixel(glm::ivec2 pixel) const;
	bool containFragMesh(const FragMesh& fragMesh) const;
	bool containOctree(std::shared_ptr<Octree> octreeRoot) const;
	bool containKDTree(std::shared_ptr<KDTree> kdTreeRoot) const;
	bool intersectKDTree(const std::shared_ptr<KDTree> kdTreeRoot) const;

	void checkPixel(glm::ivec2 pixel, float pixelDepth, glm::vec3 color, std::shared_ptr<ZBuffer> bufferPtr);
	void checkFragMesh(const FragMesh& fragMesh, const Shader& shader, std::shared_ptr<ZBuffer> bufferPtr);
	void checkOctree(std::shared_ptr<Octree> octreeRoot, const Shader& shader, std::shared_ptr<ZBuffer> bufferPtr);
	void checkKDTree(std::shared_ptr<KDTree> kdTreeRoot, const Shader& shader, std::shared_ptr<ZBuffer> bufferPtr);

	void updateQuadTreeDepth();

private:
	float depth;
	std::shared_ptr<BBOX> bboxPtr;
	std::vector<std::shared_ptr<QuadTree>> children;
};