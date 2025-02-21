#include "quad_tree.hpp"
#include "zbuffer.hpp"
#include <numeric>

QuadTree::QuadTree(BBOX& bbox) {
	depth = FLT_MAX;
	bboxPtr = std::make_shared<BBOX>(bbox);

	int minX = bbox.minX;
	int minY = bbox.minY;
	int maxX = bbox.maxX;
	int maxY = bbox.maxY;

	if (minX + 1 == maxX || minY + 1 == maxY) {
		return;
	}

	int midX = (minX + maxX) / 2;
	int midY = (minY + maxY) / 2;

	children.push_back(std::make_shared<QuadTree>(BBOX(minX, minY, midX, midY)));
	children.push_back(std::make_shared<QuadTree>(BBOX(midX, minY, maxX, midY)));
	children.push_back(std::make_shared<QuadTree>(BBOX(minX, midY, midX, maxY)));
	children.push_back(std::make_shared<QuadTree>(BBOX(midX, midY, maxX, maxY)));
}

void QuadTree::resetDepth(float d) {
	depth = d;
	for (auto& child : children) {
		child->resetDepth(d);
	}
}

void QuadTree::updateQuadTreeDepth() {
	if (children.empty())
		return;
	depth = std::accumulate(children.begin(), children.end(), FLT_MIN,
		[](float acc, const std::shared_ptr<QuadTree>& child) {
			return std::max(acc, child->depth);
		});
}

bool QuadTree::containFragMesh(const FragMesh& fragMesh) const {
	return bboxPtr->containBBox(fragMesh.bbox);
}

bool QuadTree::containPixel(glm::ivec2 pixel) const {
	return pixel.x >= bboxPtr->minX && pixel.x < bboxPtr->maxX && pixel.y >= bboxPtr->minY && pixel.y < bboxPtr->maxY;
}

bool QuadTree::containOctree(const std::shared_ptr<Octree> octreeRoot) const {
	return bboxPtr->containBBox(*(octreeRoot->getBBoxPtr()));
}

bool QuadTree::containKDTree(const std::shared_ptr<KDTree> kdTreeRoot) const
{
	return bboxPtr->containBBox(*(kdTreeRoot->getBBoxPtr()));
}

bool QuadTree::intersectKDTree(const std::shared_ptr<KDTree> kdTreeRoot) const
{
	return bboxPtr->intersectBBox(*(kdTreeRoot->getBBoxPtr()));
}

void QuadTree::checkPixel(glm::ivec2 pixel, float pixelDepth, glm::vec3 color, std::shared_ptr<ZBuffer> bufferPtr) {
	// 该像素已被消隐，无需被绘制
	if (pixelDepth >= depth)
		return;

	bool containP = false;
	for (auto& child : children) {
		if (child->containPixel(pixel)) {
			child->checkPixel(pixel, pixelDepth, color, bufferPtr);
			containP = true;
			break;
		}
	}
	// 当前节点是包含pixel的最低节点
	if (!containP) {
		if (pixelDepth < bufferPtr->getDepth(pixel.x, pixel.y))
			bufferPtr->setDepth(pixel.x, pixel.y, pixelDepth);
		if (bufferPtr->getDepth(pixel.x, pixel.y) > depth)
			depth = pixelDepth;
		bufferPtr->setPixel(pixel.x, pixel.y, color);
	}
	updateQuadTreeDepth();
}

void QuadTree::checkFragMesh(const FragMesh& fragMesh, const Shader& shader, std::shared_ptr<ZBuffer> bufferPtr) {
	// fragMesh已被消隐，无需被绘制
	if (fragMesh.bbox.minZ >= depth)
		return;

	// 判断子节点是否包含fragMesh
	bool containFM = false;
	for (auto& child : children) {
		if (child->containFragMesh(fragMesh)) {
			child->checkFragMesh(fragMesh, shader, bufferPtr);
			containFM = true;
			break;
		}
	}
	// 当前节点是包含fragMesh的最低节点
	if (!containFM) {
		// 绘制fragMesh，更新深度
		BBOX bbox = fragMesh.bbox;
		for (int x = bbox.minX; x < bbox.maxX; x++) {
			for (int y = bbox.minY; y < bbox.maxY; y++) {
				// 获取fragMesh中当前像素的深度值
				auto pixelDepth = shader.calculateDepth({ x, y }, fragMesh);
				if (pixelDepth < bufferPtr->getDepth(x, y))
					checkPixel({ x, y }, pixelDepth, fragMesh.color, bufferPtr);
			}
		}
	}
	updateQuadTreeDepth();
}

void QuadTree::checkOctree(std::shared_ptr<Octree> octreeRoot, const Shader& shader, std::shared_ptr<ZBuffer> bufferPtr) {
	if (depth <= octreeRoot->getDepth())
		return;

	// 绘制octree节点中的fragMesh
	const auto& fragMeshesPtr = octreeRoot->getFragMeshesPtr();
	for (const auto& fragMeshPtr : fragMeshesPtr) {
		checkFragMesh(*fragMeshPtr, shader, bufferPtr);
	}

	// 判断子节点是否包含octreeRoot
	for (const auto& octreeChild : octreeRoot->getChildren()) {
		for (auto& quadTreeChild : children) {
			if (quadTreeChild->containOctree(octreeChild)) {
				quadTreeChild->checkOctree(octreeChild, shader, bufferPtr);
				break;
			}
		}
	}
	updateQuadTreeDepth();
}

void QuadTree::checkKDTree(std::shared_ptr<KDTree> kdTreeRoot, const Shader& shader, std::shared_ptr<ZBuffer> bufferPtr)
{
	if (depth <= kdTreeRoot->getDepth())
		return;

	if (intersectKDTree(kdTreeRoot))
	{
		// 如果当前节点是叶子节点，则绘制fragMesh
		const auto& fragMeshesPtr = kdTreeRoot->getFragMeshesPtr();
		for (const auto& fragMeshPtr : fragMeshesPtr) {
			checkFragMesh(*fragMeshPtr, shader, bufferPtr);
		}

		for (const auto& kdTreeChild : { kdTreeRoot->getLeftChild(), kdTreeRoot->getRightChild() }) {
			if (!kdTreeChild) continue;
			checkKDTree(kdTreeChild, shader, bufferPtr);
		}
	}

	updateQuadTreeDepth();
}