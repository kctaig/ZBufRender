#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

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

    Model(std::vector<Mesh> triangles, std::vector<Vertex> vertices)
        : triangles(triangles), vertices(vertices) {}
    Model(std::string dirPath, std::string fileName);

    std::vector<Mesh> getTriangles() const { return triangles; }
    std::vector<Vertex> getVertices() const { return vertices; }
    void setVertices(std::vector<Vertex> vertices) { this->vertices = vertices; }
    void setTriangles(std::vector<Mesh> triangles) { this->triangles = triangles; }

    void modelInfo();

   private:
    std::vector<Mesh> triangles;
    std::vector<Vertex> vertices;
};
