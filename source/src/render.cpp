#include <algorithm>
#include<iostream>
#include <omp.h>

#include "bbox.hpp"
#include "model.hpp"
#include "render.hpp"

void Render::processTriangles(const Uniforms& uniforms,
	const Shader& shader,
	bool useParallel) const {
	const auto& vertices = modelPtr->getVertices();
	const auto& triangles = modelPtr->getTriangles();

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
				localFragMesh.v2d[vertexIndex++] = shader.getVertexShader()(vertex, uniforms);
			}

			// raster and shader
			regularRaster(localFragMesh, shader, uniforms);
		}
	}
}

void Render::regularRaster(const FragMesh& fragMesh,
	const Shader& shader,
	const Uniforms& uniforms) const {
	const int width = static_cast<int>(bufferPtr->getWidth());
	const int height = static_cast<int>(bufferPtr->getHeight());
	BBOX bbox({ 0, 0, width, height });
	bbox.updateBBox(fragMesh);
	for (int x = bbox.getMinX(); x < bbox.getMaxX(); x++) {
		for (int y = bbox.getMinY(); y < bbox.getMaxY(); y++) {
			renderPixel({ x, y }, fragMesh, shader, uniforms);
		}
	}
}

void Render::scanLineRender(const Shader& shader,
	const Uniforms& uniforms) const {
    auto scanLineBufferPtr = std::dynamic_pointer_cast<ScanLineZBuffer>(bufferPtr);

	// construct pat
	const auto& vertices = modelPtr->getVertices();
	const auto& triangles = modelPtr->getTriangles();
	 
	FragMesh fragMesh{ std::vector<glm::vec4>(3),std::vector<glm::vec3>(3),3 };

	// construct CPT
	for (int i = 0; i < triangles.size(); ++i) {
		const auto& tri = triangles[i];
		size_t vertexIndex = 0;
		// construct fragMesh
		for (const auto& index : tri.indices) {
			auto vertex = vertices[index];
			fragMesh.v2d[vertexIndex] = shader.getVertexShader()(vertex, uniforms);
			fragMesh.v3d[vertexIndex++] = vertex.pos;
		}
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
			float z = aetNode.zl;

			int beginX = aetNode.xl < 0 ? 0 : aetNode.xl;
			int width = scanLineBufferPtr->getWidth();
			int endX = aetNode.xr >= width ? width - 1 : aetNode.xr;

			// 增量式深度更新
			for (int x = beginX; x <= endX; x++)
			{
				glm::vec3 color = shader.getFragmentShader()(glm::vec4(x, y, 0.f, 1.f), uniforms);
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
	const glm::vec3 color = shader.getFragmentShader()(glm::vec4(x, y, 0.f, 1.f), uniforms);
	regularbufferPtr->setPixel(x, y, color);
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
		return glm::vec3(0.f); // 如果行列式太小，认为点不在三角形内
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
		return glm::vec3(-1.f); // 该点不在三角形内
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
