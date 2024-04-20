#define TINYOBJLOADER_IMPLEMENTATION
#include "app.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include "ve_vertex.h"
#include <glm/glm.hpp>
#include <tiny_obj_loader.h>

	std::vector<ve::Vertex> vertices;
	std::vector<uint16_t> indices;
	const std::string MODEL_PATH = "Models/cube.obj";
	//glm::vec3 color = {1.0f,0.0f,0.0f};
	/*void serpinksi(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int depth) {
		if (depth == 0) {
			int offset = vertices.size();
			std::vector<float> random;
			for (int i = 0; i < 6; i++) {
				random.push_back((float)rand() / (float)rand());
			}
			glm::vec3 color1 = {p1,random[0]};
			color1 = glm::normalize(color1);

			glm::vec3 color2 = { random[1],p1};
			color2 = glm::normalize(color2);

			glm::vec3 color3 = {p2,random[2]};
			color3 = glm::normalize(color3);

			glm::vec3 color4 = {random[3],p2};
			color4 = glm::normalize(color4);

			glm::vec3 color5 = { p3,random[4]};
			color5 = glm::normalize(color5);

			glm::vec3 color6 = { random[5],p3};
			color6 = glm::normalize(color6);


			
			vertices.push_back({p1,color1});
			vertices.push_back({ { (p1[0] + p2[0]) / 2,(p1[1] + p2[1]) / 2 },color2 });
			vertices.push_back({ { (p1[0] + p3[0]) / 2,(p1[1] + p3[1]) / 2 } ,color3});
			vertices.push_back({p2,color4});
			vertices.push_back({ { (p2[0] + p3[0]) / 2,(p2[1] + p3[1]) / 2 } ,color5});
			vertices.push_back({p3,color6});
			indices.push_back(offset);
			indices.push_back(offset+1);
			indices.push_back(offset+2);
			indices.push_back(offset+1);
			indices.push_back(offset+3);
			indices.push_back(offset+4);
			indices.push_back(offset+2);
			indices.push_back(offset+4);
			indices.push_back(offset+5);
			return;
		}
		depth--;
		serpinksi(p1, {(p1[0]+p2[0])/2,(p1[1]+p2[1])/2}, { (p1[0] + p3[0]) / 2,(p1[1] + p3[1]) / 2 }, depth);
		serpinksi({ (p1[0] + p2[0]) / 2,(p1[1] + p2[1]) / 2 },p2, { (p2[0] + p3[0]) / 2,(p2[1] + p3[1]) / 2 },depth);
		serpinksi({ (p1[0] + p3[0]) / 2,(p1[1] + p3[1]) / 2 }, { (p2[0] + p3[0]) / 2,(p2[1] + p3[1]) / 2 },p3,depth);
	}*/

	void loadMesh() {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if(!(tinyobj::LoadObj(&attrib,&shapes,&materials,&err,MODEL_PATH.c_str()))){
			throw std::runtime_error(err);
		}

		for(const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				ve::Vertex vertex{};
				vertex.pos = {attrib.vertices[3*index.vertex_index+0],attrib.vertices[3*index.vertex_index+1],attrib.vertices[3*index.vertex_index+2]};
				vertex.texCoord = {attrib.texcoords[2*index.texcoord_index+0],1-attrib.texcoords[2*index.texcoord_index+1]};
				vertex.normal = { attrib.normals[3 * index.normal_index + 0],attrib.normals[3 * index.normal_index + 1],attrib.normals[3 * index.normal_index + 2] };
				vertex.color = {1.0f,1.0f,1.0f};
				vertices.push_back(vertex);
				indices.push_back(indices.size());
				
			}
		}
	
	}

	int main() {


		//serpinksi({ -0.5f,0.5f }, { 0.0f,-0.5f }, {0.5,0.5},4);
		loadMesh();
		ve::app app(vertices,indices);

		try {
			app.run();
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << '\n';
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}


