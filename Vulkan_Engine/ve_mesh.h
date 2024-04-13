#pragma once
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include "ve_vertex.h"
#include "ve_device.h"

	struct VertexInputDescription {
		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;
		VkPipelineVertexInputStateCreateFlags flags = 0;

	};
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;

		static VertexInputDescription getVertexDescription();
	};

	struct Mesh {
		std::vector<Vertex> vertices; 
		std::vector<uint16_t> indices;
	};



