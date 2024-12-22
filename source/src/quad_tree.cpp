#include "quad_tree.hpp"

QuadTree::QuadTree(BBOX& bbox) {
	depth = 1.f;
	bboxPtr = std::make_shared<BBOX>(bbox);

	int minX = bbox.getMinX();
	int minY = bbox.getMinY();
	int maxX = bbox.getMaxX();
	int maxY = bbox.getMaxY();

	int midX = (minX + maxX) / 2;
	int midY = (minY + maxY) / 2;

	if (minX + 1 == maxX && minY + 1 == maxY) {
		return;
	}
	else if (minX + 1 < maxX && minY + 1 == maxY) {
		children.push_back(std::make_shared<QuadTree>(BBOX(minX, minY, midX, maxY)));
		children.push_back(std::make_shared<QuadTree>(BBOX(midX, minY, maxX, maxY)));
	}
	else if (minX + 1 == maxX && minY + 1 < maxY) {
		children.push_back(std::make_shared<QuadTree>(BBOX(minX, minY, maxX, midY)));
		children.push_back(std::make_shared<QuadTree>(BBOX(minX, midY, maxX, maxY)));
	}
	else {
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
	if (children.empty()) return;

	float tempDepth = 0.f;
	for (auto& child : children) {
		tempDepth = std::max(tempDepth, child->getDepth());
	}
	depth = tempDepth;
}

bool QuadTree::containFragMesh(const FragMesh& fragMesh) const {
	return bboxPtr->containFragMesh(fragMesh);
}

bool QuadTree::containPixel(glm::ivec2 pixel) const {
	return pixel.x >= bboxPtr->getMinX()
		&& pixel.x <= bboxPtr->getMaxX()
		&& pixel.y >= bboxPtr->getMinY()
		&& pixel.y <= bboxPtr->getMaxY();
}

void QuadTree::checkPixel(glm::ivec2 pixel, float pixelDepth, glm::vec3 color, std::shared_ptr<ZBuffer>bufferPtr) {
	// �������ѱ����������豻����
	if (pixelDepth > depth) return;

	bool flag = false;
	for (auto& child : children) {
		if (child->containPixel(pixel)) {
			child->checkPixel(pixel, pixelDepth, color, bufferPtr);
			flag = true;
			updateDepth();
			break;
		}
	}

	// ��ǰ�ڵ��ǰ���pixel����ͽڵ�
	if (!flag) {
		if (pixelDepth < depth) {
			depth = pixelDepth;
			bufferPtr->setDepth(pixel.x, pixel.y, pixelDepth);
			bufferPtr->setPixel(pixel.x, pixel.y, color);
		}
	}
}

void QuadTree::checkFragMesh(const FragMesh& fragMesh, const Shader& shader, std::shared_ptr<ZBuffer> bufferPtr) {
	// fragMesh�ѱ����������豻����
	if (fragMesh.zmin > depth)
		return;

	// �ж��ӽڵ��Ƿ����fragMesh
	bool flag = false;
	for (auto& child : children) {
		if (child->containFragMesh(fragMesh)) {
			child->checkFragMesh(fragMesh, shader, bufferPtr);
			flag = true;
			// ���µ�ǰ�ڵ�����
			updateDepth();
			break;
		}
	}
	// ��ǰ�ڵ��ǰ���fragMesh����ͽڵ�
	if (!flag) {
		// ����fragMesh���������
		for (int x = bboxPtr->getMinX(); x < bboxPtr->getMaxX(); x++) {
			for (int y = bboxPtr->getMinY(); y < bboxPtr->getMaxY(); y++) {
				// ��ȡfragMesh�е�ǰ���ص����ֵ
				auto pixelDepth = shader.calculateDepth({ x, y }, fragMesh);
				if (pixelDepth <= 1.f && pixelDepth >= 0.f)
					checkPixel({ x,y }, pixelDepth, fragMesh.color, bufferPtr);
			}
		}
	}
}