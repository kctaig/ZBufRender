#include "render.hpp"

void Render::regularRender(const Uniforms& uniforms,
	const Shader& shader) const {
	const auto& vertices = modelPtr->getVertices();
	const auto& triangles = modelPtr->getTriangles();
	std::vector<glm::vec4> screenVertices;
	for (const Vertex& vertex : vertices) {
		screenVertices.push_back(glm::vec4(vertex.pos, 1.0));
	}
	// 对所有顶点进行变换
	shader.getVertexShader()(screenVertices, uniforms);

	BBOX screenBBox{ 0, 0, static_cast<int>(bufferPtr->getWidth()),
					static_cast<int>(bufferPtr->getHeight()) };

	std::shared_ptr<FragMesh> fragMeshptr = std::make_shared<FragMesh>(3);
	for (int i = 0; i < triangles.size(); ++i) {
		const auto& tri = triangles[i];
		size_t vertexIndex = 0;
		for (const auto& index : tri.indices) {
			fragMeshptr->v3d[vertexIndex] = vertices[index].pos;
			fragMeshptr->v2d[vertexIndex] = screenVertices[index];
			vertexIndex++;
		}
		shader.getFragmentShader()(*fragMeshptr, uniforms);

		BBOX bbox(fragMeshptr->v2d);
		bbox.limitedToBBox(screenBBox);
		for (int x = bbox.minX; x < bbox.maxX; x++) {
			for (int y = bbox.minY; y < bbox.maxY; y++) {
				auto depth = shader.calculateDepth({ x, y }, *fragMeshptr);
				if (depth > bufferPtr->getDepth(x, y)) {
					continue;
				}
				// fragment shader
				bufferPtr->setPixel(x, y, fragMeshptr->color);
				bufferPtr->setDepth(x, y, depth);
			}
		}
	}
}

void Render::scanLineRender(const Shader& shader,
	const Uniforms& uniforms) const {
	auto scanLineBufferPtr = std::dynamic_pointer_cast<ScanLineZBuffer>(bufferPtr);

	const auto& vertices = modelPtr->getVertices();
	const auto& triangles = modelPtr->getTriangles();
	std::vector<glm::vec4> screenVertices;
	for (const Vertex& vertex : vertices) {
		screenVertices.push_back(glm::vec4(vertex.pos, 1.0));
	}
	// 对所有顶点进行变换
	shader.getVertexShader()(screenVertices, uniforms);

	// 建立 fragMesh
	std::shared_ptr<FragMesh> fragMeshptr = std::make_shared<FragMesh>(3);
	for (int i = 0; i < triangles.size(); ++i) {
		const auto& tri = triangles[i];
		size_t vertexIndex = 0;
		for (const auto& index : tri.indices) {
			fragMeshptr->v3d[vertexIndex] = vertices[index].pos;
			fragMeshptr->v2d[vertexIndex] = screenVertices[index];
			vertexIndex++;
		}
		shader.getFragmentShader()(*fragMeshptr, uniforms);

		// construct CPTNOde
		scanLineBufferPtr->fragMeshToCPT(*fragMeshptr, i);
	}

	// construct AET and rend scan line pixel
	for (int y = static_cast<int>(scanLineBufferPtr->getHeight()) - 1; y >= 0; y--) {
		// clear depth
		scanLineBufferPtr->clearDepth();
		// check CPT
		scanLineBufferPtr->checkCPT(y);
		// render scan line pixel
		for (int i = 0; i < scanLineBufferPtr->getAETPtr()->size(); i++) {
			auto& aetNode = scanLineBufferPtr->getAETPtr()->at(i);
			auto color = aetNode.cptNodePtr->color;
			float z = aetNode.zl;

			int beginX = aetNode.xl < 0 ? 0 : static_cast<int>(aetNode.xl);
			int width = static_cast<int>(scanLineBufferPtr->getWidth());
			int endX = aetNode.xr >= width ? width - 1 : static_cast<int>(aetNode.xr);

			// 增量式深度更新
			for (int x = beginX; x <= endX; x++) {
				if (z < scanLineBufferPtr->getDepth(x)) {
					scanLineBufferPtr->setPixel(x, y, color);
					scanLineBufferPtr->setDepth(x, z);
				}
				z += aetNode.dzx;
			}
		}
		// update AET
		scanLineBufferPtr->updateAET();
	}
}

void Render::naiveHierarchyRender(const Shader& shader,
	const Uniforms& uniforms) const {
	auto naiveHierarchyBufferPtr = std::dynamic_pointer_cast<HierarchyZBuffer>(bufferPtr);
	const auto& vertices = modelPtr->getVertices();
	const auto& triangles = modelPtr->getTriangles();
	std::vector<glm::vec4> screenVertices;
	for (const Vertex& vertex : vertices) {
		screenVertices.push_back(glm::vec4(vertex.pos, 1.0));
	}
	// 对所有顶点进行变换
	shader.getVertexShader()(screenVertices, uniforms);

	// todo: 避免重复构建四叉树

	// auto start = std::chrono::high_resolution_clock::now();
	//  建立四叉树
	std::shared_ptr<QuadTree> quadTreeRoot = std::make_shared<QuadTree>(
		BBOX{ 0, 0,
			static_cast<int>(bufferPtr->getWidth()),
			static_cast<int>(bufferPtr->getHeight()) });

	// auto end = std::chrono::high_resolution_clock::now();
	// const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	// std::cout << "construct quad_tree time: " << duration << " ms" << std::endl;

	BBOX screenBBox({ 0, 0, static_cast<int>(bufferPtr->getWidth()),
					 static_cast<int>(bufferPtr->getHeight()) });

	std::shared_ptr<FragMesh> fragMeshptr = std::make_shared<FragMesh>(3);
	for (int i = 0; i < triangles.size(); ++i) {
		const auto& tri = triangles[i];
		size_t vertexIndex = 0;
		for (const auto& index : tri.indices) {
			fragMeshptr->v3d[vertexIndex] = vertices[index].pos;
			fragMeshptr->v2d[vertexIndex] = screenVertices[index];
			vertexIndex++;
		}
		shader.getFragmentShader()(*fragMeshptr, uniforms);
		fragMeshptr->bbox = BBOX3d(fragMeshptr->v2d);
		fragMeshptr->bbox.limitedToBBox(screenBBox);
		quadTreeRoot->checkFragMesh(*fragMeshptr, shader, bufferPtr);
	}
}

void Render::octreeHierarchyRender(const Shader& shader,
	const Uniforms& uniforms) const {
	auto octreeHierarchyBufferPtr = std::dynamic_pointer_cast<HierarchyZBuffer>(bufferPtr);
	const auto& vertices = modelPtr->getVertices();
	const auto& triangles = modelPtr->getTriangles();
	std::vector<glm::vec4> screenVertices;

	for (const Vertex& vertex : vertices) {
		screenVertices.push_back(glm::vec4(vertex.pos, 1.0));
	}
	shader.getVertexShader()(screenVertices, uniforms);

	BBOX screenBBox({ 0, 0, static_cast<int>(bufferPtr->getWidth()),
					 static_cast<int>(bufferPtr->getHeight()) });

	std::vector<std::shared_ptr<FragMesh>> fragMeshesPtr;

	float minZ = FLT_MAX, maxZ = FLT_MIN;
	for (int i = 0; i < triangles.size(); ++i) {
		const auto& tri = triangles[i];
		std::shared_ptr<FragMesh> fragMeshptr = std::make_shared<FragMesh>(3);
		size_t vertexIndex = 0;
		for (const auto& index : tri.indices) {
			fragMeshptr->v3d[vertexIndex] = vertices[index].pos;
			fragMeshptr->v2d[vertexIndex] = screenVertices[index];
			vertexIndex++;
		}
		shader.getFragmentShader()(*fragMeshptr, uniforms);
		fragMeshptr->bbox = BBOX3d(fragMeshptr->v2d);
		fragMeshptr->bbox.limitedToBBox(screenBBox);
		fragMeshesPtr.push_back(fragMeshptr);
		minZ = std::min(minZ, fragMeshptr->bbox.minZ);
		maxZ = std::max(maxZ, fragMeshptr->bbox.maxZ);
	}
	std::shared_ptr<QuadTree> quadTreeRoot = std::make_shared<QuadTree>(
		BBOX{ 0, 0,
			 static_cast<int>(bufferPtr->getWidth()),
			 static_cast<int>(bufferPtr->getHeight()) });

	std::shared_ptr<Octree> octreeRoot = std::make_shared<Octree>(
		BBOX3d{ 0, 0, minZ,
			   static_cast<int>(bufferPtr->getWidth()),
			   static_cast<int>(bufferPtr->getHeight()), maxZ },
		fragMeshesPtr);
	quadTreeRoot->checkOctree(octreeRoot, shader, bufferPtr);
}