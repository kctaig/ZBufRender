#include <algorithm>
#include<iostream>
#include <omp.h>

#include "bbox.hpp"
#include "model.hpp"
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
	// std::cout << "maxThreads: " << maxThreads << std::endl;

	// assign FragMesh for thread
	std::vector<FragMesh> fragMeshes(maxThreads, FragMesh{
										 std::vector<glm::vec4>(3),
										 std::vector<glm::vec3>(3), 3
		});

#pragma omp parallel if(useParallel)
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
					renderPixel({ x, y }, localFragMesh, shader, uniforms);
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
	FragMesh fragMesh{ std::vector<glm::vec4>(3),std::vector<glm::vec3>(3),3 };
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
	for (int y = scanLineBufferPtr->getHeight() - 1; y >= 0; y--)
	{
		// clear depth
		scanLineBufferPtr->clearDepth();
		// check CPT
		scanLineBufferPtr->checkCPT(y);
		// render scan line pixel
		for (int i = 0; i < scanLineBufferPtr->getAETPtr()->size(); i++)
		{
			auto& aetNode = scanLineBufferPtr->getAETPtr()->at(i);
			auto color = aetNode.cptNodePtr->color;
			float z = aetNode.zl;

			int beginX = aetNode.xl < 0 ? 0 : aetNode.xl;
			int width = scanLineBufferPtr->getWidth();
			int endX = aetNode.xr >= width ? width - 1 : aetNode.xr;

			// 增量式深度更新
			for (int x = beginX; x <= endX; x++)
			{
				if (z < scanLineBufferPtr->getDepth(x))
				{
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

void Render::renderPixel(glm::ivec2 pixel,
	const FragMesh& fragMesh,
	const Shader& shader,
	const Uniforms& uniforms) const {
	auto regularbufferPtr = std::dynamic_pointer_cast<RegularZBuffer>(bufferPtr);
	const int x = pixel.x;
	const int y = pixel.y;
	const glm::vec2 screenPoint(static_cast<float>(x) + .5f, static_cast<float>(y) + .5f);
	glm::vec3 weights = calculateWeights(fragMesh, screenPoint);
	// 检查重心坐标是否无效
	if (std::any_of(&weights[0], &weights[0] + 3,
		[](const float w) { return w < -EPSILON; })) {
		return;
	}
	// depth test
	const float depth = fragMesh.v2d[0].z * weights[0] +
		fragMesh.v2d[1].z * weights[1] +
		fragMesh.v2d[2].z * weights[2];

	if (depth > regularbufferPtr->getDepth(x, y)) {
		return;
	}
	// fragment shader
	regularbufferPtr->setPixel(x, y, fragMesh.color);
	regularbufferPtr->setDepth(x, y, depth);
}

glm::vec3 Render::calculateWeights(const FragMesh& fragMesh,
	const glm::vec2& screenPoint) {
	glm::vec3 weights(0.f), screenWeights(0.f);
	const glm::vec4 fragCoords[3] = {
		fragMesh.v2d[0],
		fragMesh.v2d[1],
		fragMesh.v2d[2]
	};

	// 计算边向量和屏幕点向量
	const glm::vec2 ab = fragCoords[1] - fragCoords[0];
	const glm::vec2 ac = fragCoords[2] - fragCoords[0];
	const glm::vec2 ap = screenPoint - glm::vec2(fragCoords[0]);

	// 计算行列式的值，用于计算重心坐标
	const float det = ab.x * ac.y - ab.y * ac.x;

	if (std::abs(det) < EPSILON) {
		return glm::vec3(-EPSILON - 1); // 如果行列式太小，认为点不在三角形内
	}

	// 计算重心坐标
	const float factor = 1.0f / det;
	const float s = (ac.y * ap.x - ac.x * ap.y) * factor;
	const float t = (ab.x * ap.y - ab.y * ap.x) * factor;

	screenWeights[0] = 1.0f - s - t;
	screenWeights[1] = s;
	screenWeights[2] = t;

	// 如果重心坐标无效（小于0或者大于1），则跳过该点
	if (s < -EPSILON || t < -EPSILON || s + t > 1.0f + EPSILON) {
		return glm::vec3(-EPSILON - 1); // 该点不在三角形内
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