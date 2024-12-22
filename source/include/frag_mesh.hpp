#pragma once

#include <glm/glm.hpp>
#include <vector>

struct FragMesh {
	std::vector<glm::vec4> v2d;
	std::vector<glm::vec3> v3d;
	size_t vertexNum;
	float zmin = 0, zmax = 0, xmin = 0, xmax = 0, ymin = 0, ymax = 0;
	glm::vec3 color = { 1.f, 1.f, 1.f };

	void init3dBbox() {
		xmin = std::min(v3d[0].x, std::min(v3d[1].x, v3d[2].x));
		xmax = std::max(v3d[0].x, std::max(v3d[1].x, v3d[2].x));
		ymin = std::min(v3d[0].y, std::min(v3d[1].y, v3d[2].y));
		ymax = std::max(v3d[0].y, std::max(v3d[1].y, v3d[2].y));
		zmin = std::min(v3d[0].z, std::min(v3d[1].z, v3d[2].z));
		zmax = std::max(v3d[0].z, std::max(v3d[1].z, v3d[2].z));
	}

	glm::vec3 calculateV2dNormal() const {
		return normalize(glm::cross(glm::vec3(v2d[1]) - glm::vec3(v2d[0]), glm::vec3(v2d[2]) - glm::vec3(v2d[0])));
	}
	glm::vec3 calculateV3dNormal() const {
		return normalize(glm::cross(v3d[1] - v3d[0], v3d[2] - v3d[0]));
	}
};