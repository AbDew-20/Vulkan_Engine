#pragma once
#include "ve_device.h"
#include <glm/glm.hpp>
#include <vector>
namespace ve {
	struct UniformBufferObject {
		glm::mat4 model[2];
		glm::mat4 view;
		glm::mat4 proj;
	};
	class Ubo {
	public:
		Ubo(VeDevice& _veDevice,int maxFramesInFlight);
		~Ubo();

		Ubo(const Ubo&) = delete;
		Ubo& operator= (const Ubo&) = delete;
		void updateUniformBuffer(size_t currentImage,UniformBufferObject &ubo);
		inline VkBuffer getBuffer(int i) { return uniformBuffers[i]; }

	private:
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<VmaAllocation> uniformBufferAllocation;
		std::vector<void*> uniformBuffersMapped;
		VeDevice& _veDevice;
		int maxFrames;

	};
}
