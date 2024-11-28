#include "render.hpp"
#include <algorithm>
#include <chrono>
#include <model.hpp>
#include <omp.h>
#include<iostream>

void Render::draw(FrameBuffer &fb,
                  const Uniforms &uniforms,
                  const Shader &shader,
                  const std::unique_ptr<Model> &model) {
    auto start = std::chrono::high_resolution_clock::now();

    const auto &vertices = model->getVertices();
    const auto &triangles = model->getTriangles();
    const int numTriangles = static_cast<int>(triangles.size());

#define USE_OMP_FRO_PARALLEL true

#if USE_OMP_FRO_PARALLEL
    std::vector<FragMesh> fragMeshes(omp_get_max_threads(), {std::vector<glm::vec4>(3), 3});

#pragma omp parallel
    {
        const int threadId = omp_get_thread_num();

#pragma omp for nowait
        for (int i = 0; i < numTriangles; ++i) {
            const auto &tri = triangles[i];
            size_t vertexIndex = 0;
            for (const auto &index: tri.indices) {
                const auto &vertex = vertices[index];
                fragMeshes[threadId].screenMesh[vertexIndex] = shader.getVertexShader()(vertex, uniforms);
                vertexIndex++;
            }
            rasterization(fb, shader, uniforms, fragMeshes[threadId]);
        }
    }
#endif

#if USE_OMP_FRO_PARALLEL == false
    for (int i = 0; i < numTriangles; ++i) {
        FragMesh* fragMesh = new FragMesh{ std::vector<glm::vec4>(3),3 };
        const auto& tri = triangles[i];
        size_t vertexIndex = 0;
        for (const auto& index : tri.indices) {
            const auto& vertex = vertices[index];
            fragMesh->screenMesh[vertexIndex] = shader.getVertexShader()(vertex, uniforms);
            vertexIndex++;
        }
        rasterization(fb, shader, uniforms, *fragMesh);
        delete fragMesh;
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Execution time: " << duration << " ms" << std::endl;
}

void Render::processTriangles(FrameBuffer &fb,
                              const Uniforms &uniforms,
                              const Shader &shader,
                              bool useParallel) const {
    const auto &vertices = modelPtr->getVertices();
    const auto &triangles = modelPtr->getTriangles();

    // ȷ���߳���
    const int maxThreads = useParallel ? omp_get_max_threads() : 1;

    // Ϊÿ���̷߳��� FragMesh
    std::vector<FragMesh> fragMeshes(maxThreads, FragMesh{std::vector<glm::vec4>(3), 3});

#pragma omp parallel if(useParallel)
    {
        const int threadId = omp_get_thread_num();
        FragMesh &localFragMesh = fragMeshes[threadId];

#pragma omp for nowait
        for (int i = 0; i < triangles.size(); ++i) {
            const auto &tri = triangles[i];
            size_t vertexIndex = 0;

            // ������ɫ������
            for (const auto &index: tri.indices) {
                const auto &vertex = vertices[index];
                localFragMesh.screenMesh[vertexIndex] = shader.getVertexShader()(vertex, uniforms);
                vertexIndex++;
            }

            // ��դ������
            rasterization(fb, shader, uniforms, localFragMesh);
        }
    }
}

void Render::rasterization(FrameBuffer &fb,
                           const Shader &shader,
                           const Uniforms &uniforms,
                           const FragMesh &fragMesh) {
    // bounding box
    BBOX bbox({0, 0, uniforms.screenWidth, uniforms.screenHeight});
    bbox.updateBBox(fragMesh);

    for (int x = bbox.getMinX(); x < bbox.getMaxX(); x++) {
        for (int y = bbox.getMinY(); y < bbox.getMaxY(); y++) {
            glm::vec2 screenPoint(static_cast<float>(x) + .5f, static_cast<float>(y) + .5f);
            glm::vec3 weights = calculateWeights(fragMesh, screenPoint);
            // ���徲̬ epsilon
            // static constexpr float epsilon = std::numeric_limits<float>::epsilon();

            // ������������Ƿ���Ч
            if (std::any_of(&weights[0], &weights[0] + 3, [](const float w) { return w < -EPSILON; })) {
                //fb.setPixel(x, y, glm::vec3(0, 0, 1));
                continue;
            }

            // depth test
            const float fbDepth = fb.getDepth(x, y);
            const float curDepth = fragMesh.screenMesh[0].z * weights[0] + fragMesh.screenMesh[1].z * weights[1] +
                                   fragMesh.
                                   screenMesh[2].z * weights[2];
            if (curDepth > fbDepth) {
                continue;
            }
            fb.setDepth(x, y, curDepth);

            // fragment shader
            glm::vec3 color = shader.getFragmentShader()(glm::vec4(x, y, 0.f, 1.f), uniforms);
            fb.setPixel(x, y, color);
        }
    }
}

glm::vec3 Render::calculateWeights(const FragMesh &fragMesh,
                                   const glm::vec2 &screenPoint) {
    glm::vec3 weights(0.f), screenWeights(0.f);
    const glm::vec4 fragCoords[3] = {
        fragMesh.screenMesh[0],
        fragMesh.screenMesh[1],
        fragMesh.screenMesh[2]
    };

    // �������������Ļ������
    const glm::vec2 ab = fragCoords[1] - fragCoords[0];
    const glm::vec2 ac = fragCoords[2] - fragCoords[0];
    const glm::vec2 ap = screenPoint - glm::vec2(fragCoords[0]);

    // ��������ʽ��ֵ�����ڼ�����������
    const float det = ab.x * ac.y - ab.y * ac.x;

    if (std::abs(det) < EPSILON) {
        return glm::vec3(0.f); // �������ʽ̫С����Ϊ�㲻����������
    }

    // ������������
    const float factor = 1.0f / det;
    const float s = (ac.y * ap.x - ac.x * ap.y) * factor;
    const float t = (ab.x * ap.y - ab.y * ap.x) * factor;

    screenWeights[0] = 1.0f - s - t;
    screenWeights[1] = s;
    screenWeights[2] = t;

    // �������������Ч��С��0���ߴ���1�����������õ�
    if (s < -EPSILON || t < -EPSILON || s + t > 1.0f + EPSILON) {
        return glm::vec3(0.f); // �õ㲻����������
    }

    // ����ÿ������� w ֵ
    const float w0 = fragCoords[0].w * screenWeights[0];
    const float w1 = fragCoords[1].w * screenWeights[1];
    const float w2 = fragCoords[2].w * screenWeights[2];

    // ��һ����������
    const float normalizer = 1.0f / (w0 + w1 + w2);
    weights[0] = w0 * normalizer;
    weights[1] = w1 * normalizer;
    weights[2] = w2 * normalizer;

    return weights;
}
