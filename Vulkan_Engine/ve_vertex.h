#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "ve_device.h"
#include <glm/glm.hpp>
#include <array>
#include <glm/gtx/hash.hpp>
using namespace std;



namespace ve {
	

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;
		glm::vec3 normal;

		bool operator==(const Vertex& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}
		static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions;
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(Vertex,normal);

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
			VmaAllocation vertexAllocation;
			VkBuffer indexBuffer;
			VmaAllocation indexAllocation;
			VkDeviceMemory indexBufferMemory;
			VeDevice& veDevice;
		

	};

}
template<> struct hash<ve::Vertex> {
	size_t operator()(ve::Vertex const& vertex) const {
		return ((hash<glm::vec3>()(vertex.pos) ^
			(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
			((hash<glm::vec2>()(vertex.texCoord) << 1)^
			(hash<glm::vec3>()(vertex.normal)<<1)>>1);
	}
};