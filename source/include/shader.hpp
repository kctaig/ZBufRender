#pragma once

#include <iostream>
#include <algorithm>

#include "frag_mesh.hpp"
#include "uniforms.hpp"

class Shader {
public:
	Shader() = default;

	~Shader() = default;

	using vertexShader = void (*)(std::vector<glm::vec4>&, const Uniforms&);
	using fragmentShader = void (*)(FragMesh&, const Uniforms&);

	Shader(const vertexShader& vs, const fragmentShader& fs)
		: vs(vs), fs(fs) {
	}

	vertexShader getVertexShader() const { return vs; }
	fragmentShader getFragmentShader() const { return fs; }

	float calculateDepth(glm::ivec2 pixel,
		const FragMesh& fragMesh) const;

	glm::vec3 calculateWeights(const FragMesh& fragMesh,
		const glm::vec2& screenPoint)const;

	static constexpr float EPSILON = std::numeric_limits<float>::epsilon();

private:
	vertexShader vs;
	fragmentShader fs;
};

void vertexShader(std::vector<glm::vec4>& vertices, const Uniforms& uniforms);

void fragmentShader(FragMesh& fragMesh, const Uniforms& uniforms);