//
// Created by lyx on 2024/11/29.
//

#pragma once

#include "shader.hpp"
#include "buffer.hpp"

struct CETNode {
    int x, dy;
    float dx, z;

    // CETNode(const FragMesh &fragMesh, int id1, int id2) {
    //     auto a = fragMesh.v2d[id1];
    //     auto b = fragMesh.v2d[id2];
    //     x = b.x;
    //     z = fragMesh.v2d[id2].z;
    //     dy = b.y - a.y;
    //     dx = -(float) (b.x - a.x);
    //     dy;
    // }
};

struct AETNode {
};

struct PATNode {
    float a, b, c, d;
    int id, dy;
    std::unique_ptr<std::vector<CETNode> > cetPtr{};
};

struct APTNode {
    std::unique_ptr<std::vector<AETNode> > aetPtr{};
};

// class ScanLineZBuffer : public ZBuffer {
// public:
//     ScanLineZBuffer() = default;
//
//     ScanLineZBuffer(size_t width, size_t height);
//
//     void clear(const glm::vec3 &color = glm::vec3(0.0f));
//
//     auto &getPATPtr() { return patPtr; }
//     auto &getAPTPtr() { return aptPtr; }
//
//     void updatePAT(const FragMesh &fragMesh, int id) const;
//
// private:
//     std::unique_ptr<std::vector<std::vector<PATNode> > > patPtr{}; // 分类多边形表指针
//     std::unique_ptr<std::vector<std::vector<APTNode> > > aptPtr{}; // 活化多边形表指针
// };
