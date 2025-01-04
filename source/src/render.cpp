#include "render.hpp"

Render::Render(std::unique_ptr<Model> mPtr, std::shared_ptr<Camera> cPtr, std::shared_ptr<ZBuffer> fbPtr, RasterType type) {
	modelPtr = std::move(mPtr);
	cameraPtr = cPtr;
	bufferPtr = fbPtr;
	rasterType = type;
}

void Render::initFragMeshesPtr(const Uniforms& uniforms, const Shader& shader)
{
	auto start = std::chrono::high_resolution_clock::now();
	this->fragMeshesPtr.clear();
	const auto& vertices = modelPtr->getVertices();
	const auto& triangles = modelPtr->getTriangles();
	std::vector<glm::vec4> screenVertices;
	for (const Vertex& vertex : vertices) {
		screenVertices.emplace_back(vertex.pos, 1.0);
	}
	shader.getVertexShader()(screenVertices, uniforms);
	BBOX screenBBox({ 0, 0, static_cast<int>(bufferPtr->getWidth()),
					 static_cast<int>(bufferPtr->getHeight()) });
	for (const Mesh& tri : triangles) {
		std::shared_ptr<FragMesh> fragMeshptr = std::make_shared<FragMesh>(3);
		for (auto it = tri.indices.begin(); it != tri.indices.end(); ++it) {
			fragMeshptr->v3d[it - tri.indices.begin()] = vertices[*it].pos;
			fragMeshptr->v2d[it - tri.indices.begin()] = screenVertices[*it];
		}
		fragMeshptr->bbox = BBOX3d(fragMeshptr->v2d);
		fragMeshptr->bbox.limitedToBBox(screenBBox);
		shader.getFragmentShader()(*fragMeshptr, uniforms);
		this->fragMeshesPtr.push_back(fragMeshptr);
	}
	auto end = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "FragMesh Construct Time : " << duration << " ms" << std::endl;
}

void Render::regularRender(const Uniforms& uniforms,
	const Shader& shader) const {
	bufferPtr->clear({ 0.f, 0.f, 0.f });
	auto start = std::chrono::high_resolution_clock::now();
	for (auto& fragMeshPtr : fragMeshesPtr) {
		const BBOX& bbox = fragMeshPtr->bbox;
		for (int x = bbox.minX; x < bbox.maxX; x++) {
			for (int y = bbox.minY; y < bbox.maxY; y++) {
				auto depth = shader.calculateDepth({ x, y }, *fragMeshPtr);
				if (depth > bufferPtr->getDepth(x, y)) {
					continue;
				}
				bufferPtr->setPixel(x, y, fragMeshPtr->color);
				bufferPtr->setDepth(x, y, depth);
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Standard ZBuffer Rendering Time : " << duration << " ms" << std::endl;
}

void Render::scanLineRender(const Shader& shader,
	const Uniforms& uniforms) const {
	auto scanLineBufferPtr = std::dynamic_pointer_cast<ScanLineZBuffer>(bufferPtr);
	auto start = std::chrono::high_resolution_clock::now();
	for (auto it = fragMeshesPtr.begin(); it != fragMeshesPtr.end(); it++) {
		scanLineBufferPtr->fragMeshToCPT(**it, it - fragMeshesPtr.begin());
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Classification Polygon Table Construct Time: " << duration << " ms" << std::endl;

	start = std::chrono::high_resolution_clock::now();
	// construct AET and rend scan line pixel
	for (int y = static_cast<int>(bufferPtr->getHeight()) - 1; y >= 0; y--) {
		// clear depth
		scanLineBufferPtr->clearLineDepth();
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
	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "ScanLine ZBuffer Rendering Time: " << duration << " ms" << std::endl;
}

void Render::naiveHierarchyRender(const Shader& shader,
	const Uniforms& uniforms) const {
	bufferPtr->clear(glm::vec3(0.f));
	auto naiveHierarchyBufferPtr = std::dynamic_pointer_cast<HierarchyZBuffer>(bufferPtr);
	auto start = std::chrono::high_resolution_clock::now();
	for (auto& fragMeshPtr : fragMeshesPtr) {
		naiveHierarchyBufferPtr->getQuadTreeRoot()->checkFragMesh(*fragMeshPtr, shader, bufferPtr);
	}
	auto end = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Naive Hierarchy ZBuffer Rendering Time : " << duration << " ms" << std::endl;
}

void Render::octreeHierarchyRender(const Shader& shader,
	const Uniforms& uniforms) const {
	bufferPtr->clear(glm::vec3(0.f));
	auto octreeHierarchyBufferPtr = std::dynamic_pointer_cast<HierarchyZBuffer>(bufferPtr);
	float minZ = FLT_MAX, maxZ = FLT_MIN;
	for (auto& fragMeshPtr : fragMeshesPtr) {
		minZ = std::min(minZ, fragMeshPtr->bbox.minZ);
		maxZ = std::max(maxZ, fragMeshPtr->bbox.maxZ);
	}
	auto construct = std::chrono::high_resolution_clock::now();
	// construct octree
	std::shared_ptr<Octree> octreeRoot = std::make_shared<Octree>(
		BBOX3d{ 0, 0, minZ,
			   static_cast<int>(bufferPtr->getWidth()),
			   static_cast<int>(bufferPtr->getHeight()), maxZ },
		fragMeshesPtr);
	auto start = std::chrono::high_resolution_clock::now();
	// check octree
	octreeHierarchyBufferPtr->getQuadTreeRoot()->checkOctree(octreeRoot, shader, bufferPtr);
	auto end = std::chrono::high_resolution_clock::now();
	const auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(start - construct).count();
	const auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Octree Hierarchy ZBuffer Construct Time : " << duration1 << " ms" << std::endl;
	std::cout << "Rendering Time : " << duration2 << " ms" << std::endl;
}