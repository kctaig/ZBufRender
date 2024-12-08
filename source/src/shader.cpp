#include "shader.hpp"

// todo: return type to FragMesh
glm::vec4 vertexShader(Vertex& vertex, const Uniforms& uniforms) {
	// clip pos
	const glm::vec4 clipPos = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.pos, 1.f);

	// 更新裁剪后的三维坐标
	vertex.pos = glm::vec3(clipPos);

	// NDC pos
	const float w = clipPos.w;
	assert(w != 0.0f);
	glm::vec4 NDC = clipPos / w;
	NDC.w = 1.f / w;

	// fragment pos
	glm::vec4 fragPos = NDC;
	fragPos.x = (NDC.x + 1.f) * .5f * static_cast<float>(uniforms.screenWidth);
	fragPos.y = (NDC.y + 1.f) * .5f * static_cast<float>(uniforms.screenHeight);
	fragPos.z = (NDC.z + 1.f) * .5f;

	return fragPos;
}

glm::vec3 fragmentShader(const glm::vec4& fragPos, const Uniforms& uniforms) {
	return { 1, 1, 1 };
}
