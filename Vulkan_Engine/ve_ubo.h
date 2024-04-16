#pragma once
#include "ve_device.h"
#include <glm/glm.hpp>
#include <vector>
namespace ve {
	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
	class Ubo {
	public:
		Ubo(VeDevice& _veDevice,int maxFramesInFlight);
		~Ubo();
		void updateUniformBuffer(size_t currentImage,UniformBufferObject &ubo);
		inline VkBuffer getBuffer(int i) { return uniformBuffers[i]; }

	private:
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;
		VeDevice& _veDevice;
		int maxFrames;

	};
}
