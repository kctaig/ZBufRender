#include "shader.hpp"
#include <iostream>

// todo: return type to FragMesh
void vertexShader(std::vector<glm::vec4>& vertices, const Uniforms& uniforms) {
	for (auto& vertex : vertices) {
		auto clipPos = uniforms.projection * uniforms.view * uniforms.model * vertex;
		// NDC pos
		const float w = clipPos.w;
		assert(w != 0.0f);
		glm::vec4 NDC = clipPos / w;
		//NDC.w = 1.f / w;
		NDC.w = w;

		// fragment pos
		glm::vec4 fragPos = NDC;
		fragPos.x = (NDC.x + 1.f) * .5f * static_cast<float>(uniforms.screenWidth);
		fragPos.y = (NDC.y + 1.f) * .5f * static_cast<float>(uniforms.screenHeight);
		//fragPos.z = (NDC.z + 1.f) * .5f;

		//std::cout << fragPos.z << std::endl;

		fragPos.x = static_cast<int>(fragPos.x + .5f);
		fragPos.y = static_cast<int>(fragPos.y + .5f);

		vertex = fragPos;
	}
}

void fragmentShader(FragMesh& fragMesh, const Uniforms& uniforms) {
	auto normal = fragMesh.calculateV3dNormal();
	fragMesh.color = (normal + 1.f) / 2.f;
        // fragMesh.color = glm::vec3(1.f, 1.f, 1.f);
}