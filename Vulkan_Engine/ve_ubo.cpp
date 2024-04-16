#include "ve_ubo.h"
namespace ve {
	Ubo::Ubo(VeDevice& veDevice, int maxFramesInFlight) :_veDevice{ veDevice }, maxFrames{maxFramesInFlight} {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		uniformBuffers.resize(maxFrames);
		uniformBuffersMemory.resize(maxFrames);
		uniformBuffersMapped.resize(maxFrames);
		for (int i = 0; i < maxFrames; i++) {
			_veDevice.createBuffer(bufferSize,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,uniformBuffers[i],uniformBuffersMemory[i]);

			vkMapMemory(_veDevice.device(), uniformBuffersMemory[i], 0,bufferSize,0,&uniformBuffersMapped[i]);
		}

	}
	Ubo::~Ubo() {
		for (int i = 0; i < maxFrames; i++) {
			vkUnmapMemory(_veDevice.device(),uniformBuffersMemory[i]);
			vkDestroyBuffer(_veDevice.device(),uniformBuffers[i],nullptr);
			vkFreeMemory(_veDevice.device(),uniformBuffersMemory[i],nullptr);
			
		}
	}
	void Ubo::updateUniformBuffer(size_t currentImage,UniformBufferObject &ubo) {
		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}
}