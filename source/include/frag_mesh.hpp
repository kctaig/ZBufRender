#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "bbox.hpp"

struct FragMesh {
	std::vector<glm::vec4> v2d;
	std::vector<glm::vec3> v3d;
	size_t vertexNum = 3;
	BBOX3d bbox = { 0, 0, 0, 0, 0, 0 };
	glm::vec3 color = { 1.f, 1.f, 1.f };

	FragMesh() = default;

	FragMesh(size_t vertexNum);

	glm::vec3 calculateV2dNormal() const;
	glm::vec3 calculateV3dNormal() const;
};

inline FragMesh::FragMesh(size_t vertexNum) :vertexNum(vertexNum) {
	v2d.resize(vertexNum);
	v3d.resize(vertexNum);
}

inline glm::vec3 FragMesh::calculateV2dNormal() const {
	return glm::normalize(glm::cross(glm::vec3(v2d[1]) - glm::vec3(v2d[0]), glm::vec3(v2d[2]) - glm::vec3(v2d[0])));
}

inline glm::vec3 FragMesh::calculateV3dNormal() const {
	return glm::normalize(glm::cross(v3d[1] - v3d[0], v3d[2] - v3d[0]));
}