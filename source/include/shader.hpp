#pragma once

#include <vector>
#include <ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "model.hpp"

struct FragMesh {
	std::vector<glm::vec4> v2d;
	std::vector<glm::vec3> v3d;
	size_t vertexNum;

	glm::vec3 calculateV2dNormal() const {
		//return normalize(glm::cross(v3d[1] - v3d[0], v3d[2] - v3d[0]));
		return normalize(glm::cross(glm::vec3(v2d[1]) - glm::vec3(v2d[0]), glm::vec3(v2d[2]) - glm::vec3(v2d[0])));
	}
	glm::vec3 calculateV3dNormal() const {
		return normalize(glm::cross(v3d[1] - v3d[0], v3d[2] - v3d[0]));
	}
};

struct Uniforms {
	Uniforms() = default;

	~Uniforms() = default;

	Uniforms(const glm::mat4& m, const glm::mat4& v, const glm::mat4& p, const int h, const int w)
		: model(m), view(v), projection(p), screenWidth(w), screenHeight(h) {
	}

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view;
	glm::mat4 projection;

	int screenWidth;
	int screenHeight;

	void updateMVP(const Camera& cam, const size_t w, const size_t h) {
		screenWidth = static_cast<int>(w);
		screenHeight = static_cast<int>(h);
		view = cam.GetViewMatrix();
		projection = glm::perspective(glm::radians(cam.Zoom),
			static_cast<float>(screenWidth) / static_cast<float>(screenHeight),
			1.f,
			100.0f);
		//projection = glm::mat4(1.f);
	}

	void updateModel(const glm::mat4& m) {
		model = m;
	}
};

class Shader {
public:
	Shader() = default;

	~Shader() = default;

	using vertexShader = void(*)(std::vector<glm::vec4>&, const Uniforms&);
	using fragmentShader = glm::vec3(*)(const glm::vec4&, const Uniforms&);

	Shader(const vertexShader& vs, const fragmentShader& fs)
		: vs(vs), fs(fs) {
	}

	vertexShader getVertexShader() const { return vs; }
	fragmentShader getFragmentShader() const { return fs; }

private:
	vertexShader vs;
	fragmentShader fs;
};

//glm::vec4 vertexShader(Vertex& vertex, const Uniforms& uniforms);
void vertexShader(std::vector<glm::vec4>& vertices, const Uniforms& uniforms);

glm::vec3 fragmentShader(const glm::vec4& fragPos, const Uniforms& uniforms);