#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "bbox.hpp"

struct FragMesh {
	std::vector<glm::vec4> v2d;
	std::vector<glm::vec3> v3d;
	size_t vertexNum;
	BBOX3d bbox;
	glm::vec3 color = { 1.f, 1.f, 1.f };

	glm::vec3 calculateV2dNormal() const;
	glm::vec3 calculateV3dNormal() const;
};

inline glm::vec3 FragMesh::calculateV2dNormal() const {
	return normalize(glm::cross(glm::vec3(v2d[1]) - glm::vec3(v2d[0]), glm::vec3(v2d[2]) - glm::vec3(v2d[0])));
}

inline glm::vec3 FragMesh::calculateV3dNormal() const {
	return normalize(glm::cross(v3d[1] - v3d[0], v3d[2] - v3d[0]));
}