#include "ve_mesh.h"


	VertexInputDescription Vertex::getVertexDescription() {
		VertexInputDescription description;
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		description.bindings.push_back(bindingDescription);

		VkVertexInputAttributeDescription positionAttribute{};
		positionAttribute.binding = 0;
		positionAttribute.location = 0;
		positionAttribute.format = VK_FORMAT_R32G32_SFLOAT;
		positionAttribute.offset = offsetof(Vertex,pos);

		VkVertexInputAttributeDescription colorAttribute{};
		colorAttribute.binding = 0;
		colorAttribute.location = 1;
		colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		colorAttribute.offset = offsetof(Vertex, color);

		description.attributes.push_back(positionAttribute);
		description.attributes.push_back(colorAttribute);
		return description;
	}

