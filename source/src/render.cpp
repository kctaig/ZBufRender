#include <model.hpp>
#include <render.hpp>
#include <omp.h>
#include <chrono>
#include <algorithm>

void Render::draw(FrameBuffer& fb,
                  Uniforms& uniforms,
                  const Shader& shader,
                  const std::unique_ptr<Model>& model) {

	auto start = std::chrono::high_resolution_clock::now();

    const auto& vertices = model->getVertices();
    const auto& triangles = model->getTriangles();
    const int numTriangles = triangles.size();

#define USE_OMP_FRO_PARALLEL true

#if USE_OMP_FRO_PARALLEL 
    std::vector<FragMesh> fragMeshes(omp_get_max_threads(), { std::vector<glm::vec4>(3),3 });

#pragma omp parallel
    {
        int threadId = omp_get_thread_num();

#pragma omp for nowait
        for (int i = 0; i < numTriangles; ++i) {
            const auto& tri = triangles[i];
            size_t vertexIndex = 0;
            for (const auto& index : tri.indices) {
                const auto& vertex = vertices[index];
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

void Render::processTriangles(FrameBuffer& fb,
                              Uniforms& uniforms,
                              const Shader& shader,
                              const std::unique_ptr<Model>& model,
                              bool useParallel) {
    const auto& vertices = model->getVertices();
    const auto& triangles = model->getTriangles();

    // 确定线程数
    int maxThreads = useParallel ? omp_get_max_threads() : 1;

    // 为每个线程分配 FragMesh
    std::vector<FragMesh> fragMeshes(maxThreads, FragMesh{ std::vector<glm::vec4>(3), 3 });

#pragma omp parallel if(useParallel)
    {
        int threadId = omp_get_thread_num();
        FragMesh& localFragMesh = fragMeshes[threadId];

#pragma omp for nowait
        for (int i = 0; i < triangles.size(); ++i) {
            const auto& tri = triangles[i];
            size_t vertexIndex = 0;

            // 顶点着色器处理
            for (const auto& index : tri.indices) {
                const auto& vertex = vertices[index];
                localFragMesh.screenMesh[vertexIndex] = shader.getVertexShader()(vertex, uniforms);
                vertexIndex++;
            }

            // 光栅化处理
            rasterization(fb, shader, uniforms, localFragMesh);
        }
    }
}

void Render::rasterization(FrameBuffer& fb,
                           const Shader& shader,
                           const Uniforms& uniforms,
                           const FragMesh& fragMesh) {
    // bounding box
    BBOX bbox({0, 0, uniforms.screenWidth, uniforms.screenHeight});
    bbox.updateBBox(fragMesh);

    for (int x = bbox.getMinX(); x < bbox.getMaxX(); x++) {
        for (int y = bbox.getMinY(); y < bbox.getMaxY(); y++) {
            glm::vec2 screenPoint(x + .5f, y + .5f);
            glm::vec3 weights = calculateWeights(fragMesh, screenPoint);
            // 定义静态 epsilon
            static constexpr float epsilon = std::numeric_limits<float>::epsilon();

            // 检查重心坐标是否无效
            if (std::any_of(&weights[0], &weights[0] + 3, [](float w) { return w < -epsilon; })) {
                //fb.setPixel(x, y, glm::vec3(0, 0, 1));
                continue;
            }

			// depth test
			float fbDepth = fb.getDepth(x, y);
            float curDepth = fragMesh.screenMesh[0].z * weights[0] + fragMesh.screenMesh[1].z * weights[1] + fragMesh.screenMesh[2].z * weights[2];
			//if (curDepth < 1) {
   //             std::cout << x << " " << y << " " << curDepth << std::endl;
			//}
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

glm::vec3 Render::calculateWeights(const FragMesh& fragMesh,
                                   const glm::vec2& screenPoint) {
    glm::vec3 weights(0.f), screenWeights(0.f);
    glm::vec4 fragCoords[3] = { fragMesh.screenMesh[0],
                                fragMesh.screenMesh[1],
                                fragMesh.screenMesh[2] };
    // 计算边向量和屏幕点向量
    glm::vec2 ab = fragCoords[1] - fragCoords[0];
    glm::vec2 ac = fragCoords[2] - fragCoords[0];
    glm::vec2 ap = screenPoint - glm::vec2(fragCoords[0]);

    // 计算行列式的值，用于计算重心坐标
    float det = ab.x * ac.y - ab.y * ac.x;

    // 设置小的 epsilon 值，用于避免浮点误差导致的无效重心坐标
    static constexpr float epsilon = std::numeric_limits<float>::epsilon();
    if (std::abs(det) < epsilon) {
        return glm::vec3(0.f);  // 如果行列式太小，认为点不在三角形内
    }

    // 计算重心坐标
    float factor = 1.0f / det;
    double s = (ac.y * ap.x - ac.x * ap.y) * factor;
    double t = (ab.x * ap.y - ab.y * ap.x) * factor;

    screenWeights[0] = 1.0f - s - t;
    screenWeights[1] = s;
    screenWeights[2] = t;

    // 如果重心坐标无效（小于0或者大于1），则跳过该点
    if (s < -epsilon || t < -epsilon || s + t > 1.0f + epsilon) {
        return glm::vec3(0.f);  // 该点不在三角形内
    }

    // 计算每个顶点的 w 值
    float w0 = fragCoords[0].w * screenWeights[0];
    float w1 = fragCoords[1].w * screenWeights[1];
    float w2 = fragCoords[2].w * screenWeights[2];

    // 归一化重心坐标
    float normalizer = 1.0f / (w0 + w1 + w2);
    weights[0] = w0 * normalizer;
    weights[1] = w1 * normalizer;
    weights[2] = w2 * normalizer;

    return weights;
}
