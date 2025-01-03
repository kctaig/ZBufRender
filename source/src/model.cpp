#include "model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader/tiny_obj_loader.h"

Model::Model(std::string dirPath, std::string fileName) {
	tinyobj::ObjReaderConfig reader_config;
	reader_config.mtl_search_path = dirPath;  // Path to material files
	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(dirPath + "/" + fileName, reader_config)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error();
		}
		exit(1);
	}

	//if (!reader.Warning().empty()) {
	//	std::cout << "TinyObjShapesReader: " << reader.Warning();
	//}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	// add model vertices
	for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
		const auto point = glm::vec3(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]);
		this->vertices.push_back({ point });
	}

	// 将模型数据保存到Model中
	for (const auto& shape : shapes) {
		size_t mesh_vertex_offset = 0;  // 面片顶点偏移量
		const size_t mesh_num = shape.mesh.num_face_vertices.size();
		for (size_t m = 0; m < mesh_num; m++) {
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
			size_t each_mesh_vertex_num = shape.mesh.num_face_vertices[m];
			mesh->indices.resize(each_mesh_vertex_num);
			tinyobj::index_t idx;
			// 查看每个面片的顶点
			for (size_t v = 0; v < each_mesh_vertex_num; v++) {
				idx = shape.mesh.indices[mesh_vertex_offset + v];
				mesh->indices[v] = idx.vertex_index;  // 面片的的顶点索引
				if (idx.normal_index >= 0 && idx.normal_index < this->vertices.size()) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
					this->vertices[idx.normal_index].normal = glm::vec3(nx, ny, nz);  // 添加法向量
				}
			}
			this->triangles.push_back(*mesh);
			mesh_vertex_offset += each_mesh_vertex_num;
		}
	}
}

void Model::modelInfo() const {
	// print model info
	std::cout << "# of vertices  : " << (this->vertices.size()) << std::endl;
	std::cout << "# of meshes   : " << (this->triangles.size()) << std::endl;

	// print vertices
	std::cout << "start print vertices: \n";
	for (const auto& v : this->vertices) {
		std::cout << v.pos[0] << " " << v.pos[1] << " " << v.pos[2] << std::endl;
	}

	// print mesh
	std::cout << "start print meshes: \n";
	for (size_t j = 0; j < this->triangles.size(); j++) {
		std::cout << "mesh " << j << " : ";
		std::cout << this->triangles[j].indices[0] << " ";
		std::cout << this->triangles[j].indices[1] << " ";
		std::cout << this->triangles[j].indices[2] << std::endl;
	}
}