#include "shader.hpp"

// todo: return type to FragMesh
void vertexShader(std::vector<glm::vec4>& vertices, const Uniforms& uniforms) {
	for (auto& vertex : vertices) {
		auto clipPos = uniforms.projection * uniforms.view * uniforms.model * vertex;
		// NDC pos
		const float w = clipPos.w;
		assert(w != 0.0f);
		glm::vec4 NDC = clipPos / w;
		NDC.w = w;

		// fragment pos
		glm::vec4 fragPos = NDC;
		fragPos.x = (NDC.x + 1.f) * .5f * static_cast<float>(uniforms.screenWidth);
		fragPos.y = (NDC.y + 1.f) * .5f * static_cast<float>(uniforms.screenHeight);

		fragPos.x = static_cast<int>(fragPos.x + .5f);
		fragPos.y = static_cast<int>(fragPos.y + .5f);

		vertex = fragPos;
	}
}

void fragmentShader(FragMesh& fragMesh, const Uniforms& uniforms) {
	auto normal = fragMesh.calculateV3dNormal();
	fragMesh.color = (normal + 1.f) / 2.f;
	//fragMesh.color = glm::vec3(1.f, 1.f, 1.f);
}

float Shader::calculateDepth(glm::ivec2 pixel,
	const FragMesh& fragMesh) const {
	const int x = pixel.x;
	const int y = pixel.y;
	const glm::vec2 screenPoint(static_cast<float>(x) + .5f, static_cast<float>(y) + .5f);
	glm::vec3 weights = calculateWeights(fragMesh, screenPoint);
	// 检查重心坐标是否无效
	if (std::any_of(&weights[0], &weights[0] + 3,
		[](const float w) { return w < -EPSILON; })) {
		return INT_MAX;
	}
	// calculate test
	float depth = fragMesh.v2d[0].z * weights[0] +
		fragMesh.v2d[1].z * weights[1] +
		fragMesh.v2d[2].z * weights[2];

	return depth;
}

glm::vec3 Shader::calculateWeights(const FragMesh& fragMesh,
	const glm::vec2& screenPoint) const {
	glm::vec3 weights(0.f), screenWeights(0.f);
	const glm::vec4 fragCoords[3] = {
		fragMesh.v2d[0],
		fragMesh.v2d[1],
		fragMesh.v2d[2] };

	// 计算边向量和屏幕点向量
	const glm::vec2 ab = fragCoords[1] - fragCoords[0];
	const glm::vec2 ac = fragCoords[2] - fragCoords[0];
	const glm::vec2 ap = screenPoint - glm::vec2(fragCoords[0]);

	// 计算行列式的值，用于计算重心坐标
	const float det = ab.x * ac.y - ab.y * ac.x;

	if (std::abs(det) < EPSILON) {
		return glm::vec3(-EPSILON - 1);  // 如果行列式太小，认为点不在三角形内
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
		return glm::vec3(-EPSILON - 1);  // 该点不在三角形内
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