#pragma once
#include "ve_device.h"
#include <glm/glm.hpp>
#include <array>
namespace ve {
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}
		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions;
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}
	};

	class VertexBuffer {
		public:
			VertexBuffer(VeDevice &device, std::vector<Vertex> &vertices, std::vector<uint16_t> &indices);
			~VertexBuffer();
			VertexBuffer(const VertexBuffer&) = delete;
			VertexBuffer& operator=(const VertexBuffer&) = delete;

			inline VkBuffer& getVertBuffer() { return vertexBuffer; }
			inline VkBuffer& getIndexBuffer() { return indexBuffer; }
			inline uint32_t getVerticesNum () { return vertNums; }

		private:
			void createVertexBuffer(std::vector<Vertex>& vertices);
			void createIndexBuffer(std::vector<uint16_t>& indices);

			uint32_t vertNums;
			VkBuffer vertexBuffer;
			VkDeviceMemory vertexBufferMemory;
			VkBuffer indexBuffer;
			VkDeviceMemory indexBufferMemory;
			VeDevice& veDevice;
		

	};

}