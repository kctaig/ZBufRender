#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
};

struct Mesh {
    std::vector<size_t> indices;
};

class Model {
public:
    Model() = default;

    ~Model() = default;

    Model( std::string dirPath,  std::string fileName);

    const std::vector<Mesh> &getTriangles() const { return triangles; }
    const std::vector<Vertex> &getVertices() const { return vertices; }
    void setVertices(const std::vector<Vertex> &vertices) { this->vertices = vertices; }
    void setTriangles(const std::vector<Mesh> &triangles) { this->triangles = triangles; }

    void modelInfo() const;

private:
    std::vector<Mesh> triangles;
    std::vector<Vertex> vertices;
};
