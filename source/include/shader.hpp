#pragma once

#include <iostream>
#include <algorithm>

#include "frag_mesh.hpp"
#include "uniforms.hpp"

class Shader {
public:
	Shader() = default;

	~Shader() = default;

	using VertexShader = void (*)(std::vector<glm::vec4>&, const Uniforms&);
	using FragmentShader = void (*)(FragMesh&, const Uniforms&);

	Shader(const VertexShader& vs, const FragmentShader& fs)
		: vertexShader(vs), fragmentShader(fs) {
	}

	float calculateDepth(glm::ivec2 pixel,
		const FragMesh& fragMesh) const;

	glm::vec3 calculateWeights(const FragMesh& fragMesh,
		const glm::vec2& screenPoint)const;

	static constexpr float EPSILON = std::numeric_limits<float>::epsilon();

	static void vs(std::vector<glm::vec4>& vertices, const Uniforms& uniforms);

	static void fs(FragMesh& fragMesh, const Uniforms& uniforms);

	VertexShader vertexShader;
	FragmentShader fragmentShader;
};