#include "render.hpp"

void Render::regularRender(const Uniforms& uniforms,
	const Shader& shader,
	bool useParallel) const {
	const auto& vertices = modelPtr->getVertices();
	const auto& triangles = modelPtr->getTriangles();
	std::vector<glm::vec4> screenVertices;
	for (const Vertex& vertex : vertices) {
		screenVertices.push_back(glm::vec4(vertex.pos, 1.0));
	}
	// 对所有顶点进行变换
	shader.getVertexShader()(screenVertices, uniforms);

	// number of thread
	const int maxThreads = useParallel ? omp_get_max_threads() : 1;
	//std::cout << "maxThreads: " << maxThreads << std::endl;

	// assign FragMesh for thread
	std::vector<FragMesh> fragMeshes(maxThreads, FragMesh{
													 std::vector<glm::vec4>(3),
													 std::vector<glm::vec3>(3), 3 });

#pragma omp parallel if (useParallel)
	{
		const int threadId = omp_get_thread_num();
		FragMesh& localFragMesh = fragMeshes[threadId];

#pragma omp for nowait
		for (int i = 0; i < triangles.size(); ++i) {
			const auto& tri = triangles[i];
			size_t vertexIndex = 0;
			// vertex shader
			for (const auto& index : tri.indices) {
				auto vertex = vertices[index];
				localFragMesh.v2d[vertexIndex] = screenVertices[index];
				localFragMesh.v3d[vertexIndex] = vertex.pos;
				vertexIndex++;
			}
			shader.getFragmentShader()(localFragMesh, uniforms);

			BBOX bbox({ 0, 0, static_cast<int>(bufferPtr->getWidth()),
					   static_cast<int>(bufferPtr->getHeight()) });
			bbox.updateBBox(localFragMesh);
			for (int x = bbox.getMinX(); x < bbox.getMaxX(); x++) {
				for (int y = bbox.getMinY(); y < bbox.getMaxY(); y++) {
					auto depth = shader.calculateDepth({ x, y }, localFragMesh);
					if (depth > bufferPtr->getDepth(x, y)) {
						continue;
					}
					// fragment shader
					bufferPtr->setPixel(x, y, localFragMesh.color);
					bufferPtr->setDepth(x, y, depth);
				}
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
	FragMesh fragMesh{ std::vector<glm::vec4>(3), std::vector<glm::vec3>(3), 3 };
	for (int i = 0; i < triangles.size(); ++i) {
		const auto& tri = triangles[i];
		size_t vertexIndex = 0;
		// construct fragMesh
		for (const auto& index : tri.indices) {
			fragMesh.v3d[vertexIndex] = vertices[index].pos;
			fragMesh.v2d[vertexIndex] = screenVertices[index];
			vertexIndex++;
		}

		shader.getFragmentShader()(fragMesh, uniforms);

		// construct CPTNOde
		scanLineBufferPtr->fragMeshToCPT(fragMesh, i);
	}

	// construct AET and rend scan line pixel
	for (int y = scanLineBufferPtr->getHeight() - 1; y >= 0; y--) {
		// clear depth
		scanLineBufferPtr->clearDepth();
		// check CPT
		scanLineBufferPtr->checkCPT(y);
		// render scan line pixel
		for (int i = 0; i < scanLineBufferPtr->getAETPtr()->size(); i++) {
			auto& aetNode = scanLineBufferPtr->getAETPtr()->at(i);
			auto color = aetNode.cptNodePtr->color;
			float z = aetNode.zl;

			int beginX = aetNode.xl < 0 ? 0 : aetNode.xl;
			int width = scanLineBufferPtr->getWidth();
			int endX = aetNode.xr >= width ? width - 1 : aetNode.xr;

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
	const Uniforms& uniforms,
	bool useParallel) const {
	auto naiveHierarchyBufferPtr = std::dynamic_pointer_cast<NaiveHierarchyZBuffer>(bufferPtr);
	const auto& vertices = modelPtr->getVertices();
	const auto& triangles = modelPtr->getTriangles();
	std::vector<glm::vec4> screenVertices;
	for (const Vertex& vertex : vertices) {
		screenVertices.push_back(glm::vec4(vertex.pos, 1.0));
	}
	// 对所有顶点进行变换
	shader.getVertexShader()(screenVertices, uniforms);

	// 建立四叉树
	std::shared_ptr<QuadTree> root = std::make_shared<QuadTree>(BBOX{ 0,0,
		static_cast<int>(bufferPtr->getWidth()),
		static_cast<int>(bufferPtr->getHeight())
		});

	const int maxThreads = useParallel ? omp_get_max_threads() : 1;
	std::vector<FragMesh> fragMeshes(maxThreads, FragMesh{
													 std::vector<glm::vec4>(3),
													 std::vector<glm::vec3>(3), 3 });

	BBOX screenBBox({ 0, 0, static_cast<int>(bufferPtr->getWidth()),
					   static_cast<int>(bufferPtr->getHeight()) });

#pragma omp parallel if (useParallel)
	{
		const int threadId = omp_get_thread_num();
		FragMesh& localFragMesh = fragMeshes[threadId];

#pragma omp for nowait
		for (int i = 0; i < triangles.size(); ++i) {
			const auto& tri = triangles[i];
			size_t vertexIndex = 0;
			for (const auto& index : tri.indices) {
				localFragMesh.v3d[vertexIndex] = vertices[index].pos;
				localFragMesh.v2d[vertexIndex] = screenVertices[index];
				vertexIndex++;
			}
			shader.getFragmentShader()(localFragMesh, uniforms);
			localFragMesh.init3dBbox();
			// clip fragMesh
			if (!screenBBox.containFragMesh(localFragMesh)) {
				localFragMesh.xmin = std::max(static_cast<int>(localFragMesh.xmin), screenBBox.getMinX());
				localFragMesh.ymin = std::max(static_cast<int>(localFragMesh.ymin), screenBBox.getMinY());
				localFragMesh.xmax = std::min(static_cast<int>(localFragMesh.xmax), screenBBox.getMaxX());
				localFragMesh.ymax = std::min(static_cast<int>(localFragMesh.ymax), screenBBox.getMaxY());
			}
			root->checkFragMesh(localFragMesh, shader, bufferPtr);
		}
	}
}