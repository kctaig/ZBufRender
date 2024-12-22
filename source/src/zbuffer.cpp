#include "zbuffer.hpp"
#include <algorithm>

/**************************** ZBuffer *******************************/

glm::u8vec3 ZBuffer::toU8Vec3(const glm::vec3& color) {
    return glm::u8vec3{
        static_cast<unsigned char>(glm::clamp(color.x * 255.0f, 0.0f, 255.0f)),
        static_cast<unsigned char>(glm::clamp(color.y * 255.0f, 0.0f, 255.0f)),
        static_cast<unsigned char>(glm::clamp(color.z * 255.0f, 0.0f, 255.0f))};
}

/**************************** RegularZBuffer *******************************/

RegularZBuffer::RegularZBuffer(size_t width, size_t height)
    : ZBuffer(width, height), pixelCount(width * height) {
    depthPtr = std::make_unique<std::vector<float>>(pixelCount, 1.f);
    pixelPtr = std::make_unique<std::vector<glm::u8vec3>>(pixelCount, glm::u8vec3{0, 0, 0});
}

void RegularZBuffer::clear(glm::vec3 color) {
    depthPtr->assign(pixelCount, 1.f);
    pixelPtr->assign(pixelCount, toU8Vec3(color));
}

void RegularZBuffer::bufferResize(size_t w, size_t h, glm::vec3 color) {
    width = w;
    height = h;
    pixelCount = w * h;
    depthPtr->resize(pixelCount, 1.f);
    pixelPtr->resize(pixelCount, toU8Vec3(color));
}

/**************************** ScanLineZBuffer *******************************/

ScanLineZBuffer::ScanLineZBuffer(size_t width, size_t height)
    : ZBuffer(width, height) {
    int pixelcount = width * height;
    depthPtr = std::make_unique<std::vector<float>>(width, 1.f);
    pixelPtr = std::make_unique<std::vector<glm::u8vec3>>(width * height, glm::u8vec3(0));
    cptPtr = std::make_unique<std::vector<std::vector<CPTNode>>>(height);
    aetPtr = std::make_unique<std::vector<AETNode>>();
}

void ScanLineZBuffer::clear(glm::vec3 color) {
    for (auto& innerVec : *cptPtr) {
        innerVec.clear();
    }
    aetPtr->clear();
    depthPtr->assign(width, 1.f);
    pixelPtr->assign(width * height, toU8Vec3(color));
}

void ScanLineZBuffer::fragMeshToCPT(const FragMesh& fragMesh,
                                    const int id) const {
    auto v2dNormal = fragMesh.calculateV2dNormal();
    auto v3dNormal = fragMesh.calculateV3dNormal();
    // if (v2dNormal.z == 0.f) return;

    auto v2d = fragMesh.v2d;
    auto v3d = fragMesh.v3d;

    // sort by y ascending
    if (v2d[0].y > v2d[1].y) {
        std::swap(v2d[0], v2d[1]);
        std::swap(v3d[0], v3d[1]);
    }
    if (v2d[0].y > v2d[2].y) {
        std::swap(v2d[0], v2d[2]);
        std::swap(v3d[0], v3d[2]);
    }
    if (v2d[1].y > v2d[2].y) {
        std::swap(v2d[1], v2d[2]);
        std::swap(v3d[1], v3d[2]);
    }

    CPTNode cptNode;
    cptNode.a = v2dNormal.x;
    cptNode.b = v2dNormal.y;
    cptNode.c = v2dNormal.z;
    cptNode.id = id;
    // cptNode.d = -glm::dot(normal, v3d[0]);
    cptNode.d = -glm::dot(v2dNormal, glm::vec3(v2d[0]));
    cptNode.color = fragMesh.color;

    int ymax = v2d[2].y;

    if (ymax >= height || ymax < 0)
        return;

    cptNode.dy = ymax - v2d[0].y;

    cptNode.cetPtr = std::make_shared<std::vector<CETNode>>();
    if (v2d[0].y != v2d[2].y)
        cptNode.cetPtr->push_back(CETNode(v2d[0], v2d[2]));
    if (v2d[1].y != v2d[2].y)
        cptNode.cetPtr->push_back(CETNode(v2d[1], v2d[2]));
    if (v2d[0].y != v2d[1].y)
        cptNode.cetPtr->push_back(CETNode(v2d[0], v2d[1]));
    cptPtr->at(ymax).push_back(cptNode);
}

void ScanLineZBuffer::checkCPT(int y) const {
    for (int i = 0; i < cptPtr->at(y).size(); i++) {
        const CPTNode& cptNode = cptPtr->at(y)[i];

        // continue if not tow edges
        if (cptNode.cetPtr->size() < 2)
            continue;

        int leftIdx = 0, rightIdx = 1;
        if (cptNode.cetPtr->at(1).x == cptNode.cetPtr->at(0).x) {
            if (cptNode.cetPtr->at(1).dx < cptNode.cetPtr->at(0).dx) {
                leftIdx = 1;
                rightIdx = 0;
            }
        } else if ((*cptNode.cetPtr)[1].x < (*cptNode.cetPtr)[0].x) {
            leftIdx = 1;
            rightIdx = 0;
        }

        AETNode aetNode;

        aetNode.dzx = -cptNode.a / cptNode.c;
        aetNode.dzy = cptNode.b / cptNode.c;

        // left edge
        aetNode.xl = cptNode.cetPtr->at(leftIdx).x;
        aetNode.dxl = cptNode.cetPtr->at(leftIdx).dx;
        aetNode.dyl = cptNode.cetPtr->at(leftIdx).dy;
        // right edge
        aetNode.xr = cptNode.cetPtr->at(rightIdx).x;
        aetNode.dxr = cptNode.cetPtr->at(rightIdx).dx;
        aetNode.dyr = cptNode.cetPtr->at(rightIdx).dy;

        aetNode.zl = cptNode.cetPtr->at(leftIdx).z;

        aetNode.cptNodePtr = std::make_shared<CPTNode>(cptNode);
        aetPtr->push_back(aetNode);
    }
}

void ScanLineZBuffer::updateAET() {
    for (int i = 0; i < aetPtr->size(); i++) {
        auto& aetNode = aetPtr->at(i);
        aetNode.dyl--;
        aetNode.dyr--;
        if (aetNode.dyl < 0 && aetNode.dyr < 0) {
            aetPtr->erase(aetPtr->begin() + i);
            i--;
        } else {
            // 如果边对中有一个已经渲染完，则添加mesh的下一条边
            auto cptNodePtr = aetNode.cptNodePtr;
            if (aetNode.dyl < 0) {
                aetNode.xl = cptNodePtr->cetPtr->at(2).x;
                aetNode.dxl = cptNodePtr->cetPtr->at(2).dx;
                aetNode.dyl = cptNodePtr->cetPtr->at(2).dy - 1;
            }
            if (aetNode.dyr < 0) {
                aetNode.xr = cptNodePtr->cetPtr->at(2).x;
                aetNode.dxr = cptNodePtr->cetPtr->at(2).dx;
                aetNode.dyr = cptNodePtr->cetPtr->at(2).dy - 1;
            }

            aetNode.xl += aetNode.dxl;
            aetNode.xr += aetNode.dxr;
            aetNode.zl += aetNode.dzy + aetNode.dzx * aetNode.dxl;
        }
    }
}

void ScanLineZBuffer::bufferResize(size_t w, size_t h, glm::vec3 color) {
    width = w;
    height = h;
    depthPtr->resize(w);
    pixelPtr->resize(w * h, toU8Vec3(color));
    cptPtr->resize(h);
}

/**************************** NaiveZBuffer *******************************/

NaiveHierarchyZBuffer::NaiveHierarchyZBuffer(size_t width, size_t height)
    : ZBuffer(width, height) {
    depthPtr = std::make_unique<std::vector<float>>(width * height, 1.f);
    pixelPtr = std::make_unique<std::vector<glm::u8vec3>>(width * height, glm::u8vec3(0));
}

void NaiveHierarchyZBuffer::clear(glm::vec3 color) {
    depthPtr->assign(width * height, 1.f);
    pixelPtr->assign(width * height, toU8Vec3(color));
}

void NaiveHierarchyZBuffer::bufferResize(size_t w, size_t h, glm::vec3 color) {
    width = w;
    height = h;
    depthPtr->resize(w * h, 1.f);
    pixelPtr->resize(w * h, toU8Vec3(color));
}