//
// Created by lyx on 2024/11/29.
//
#include "scan_line_buffer.hpp"
#include <algorithm>

// ScanLineZBuffer::ScanLineZBuffer(size_t width, size_t height) : ZBuffer(width, height) {
//     depthPtr = std::make_unique<std::vector<float> >(width, 1);
//     pixelPtr = std::make_unique<std::vector<glm::u8vec3> >(width, glm::u8vec3(0));
//     patPtr = std::make_unique<std::vector<std::vector<PATNode> > >(height);
//     aptPtr = std::make_unique<std::vector<std::vector<APTNode> > >(height);
// }
//
// void ScanLineZBuffer::clear(const glm::vec3 &color) {
//     depthPtr->clear();
//     pixelPtr->clear();
//     patPtr->clear();
//     aptPtr->clear();
//     depthPtr->resize(width, 1.f);
//     pixelPtr->resize(width, toU8Vec3(color));
//     patPtr->resize(height);
//     patPtr->resize(height);
// }

// void ScanLineZBuffer::updatePAT(const FragMesh &fragMesh, const int id) const {
    // auto normal = fragMesh.calculateNormal();
    // if (normal.z == 0.f) return;
    //
    // auto v2d = fragMesh.v2d;
    // auto v3d = fragMesh.v3d;
    // std::vector<std::pair<glm::vec4, glm::vec3> > paired;
    // for (size_t i = 0; i < v2d.size(); ++i) {
    //     paired.emplace_back(v2d[i], v3d[i]);
    // }
    //
    // // Sort the paired vector
    // std::sort(paired.begin(), paired.end(),
    //           [](const std::pair<glm::vec4, glm::vec3> &a, const std::pair<glm::vec4, glm::vec3> &b) {
    //               return a.first.y() < b.first.y();
    //           });
    //
    // // Unzip the sorted vectors back into their original containers
    // for (size_t i = 0; i < paired.size(); ++i) {
    //     v2d[i] = paired[i].first;
    //     v3d[i] = paired[i].second;
    // }
    // PATNode patNode;
    // patNode.a = normal.x();
    // patNode.b = normal.y();
    // patNode.c = normal.z();
    // patNode.id = id;
    // patNode.d = -glm::dot(normal, v3d[0]);
    //
    // int ymax = v2d[2].y();
    //
    // if (ymax >= height || ymax < 0) return;
    // patNode.dy = ymax - v2d[0].y();
    //
    // patNode.cetPtr->clear();
    // if (v2d[0].y() != v2d[2].y())
    //     patNode.cetPtr->push_back(CETNode(fragMesh, 0, 2));
    // if (v2d[1].y() != v2d[2].y())
    //     patNode.cetPtr->push_back(CETNode(fragMesh, 1, 2));
    // if (v2d[0].y() != v2d[1].y())
    //     patNode.cetPtr->push_back(CETNode(fragMesh, 0, 1));
    // (*patPtr)[ymax].push_back(patNode);
// }
