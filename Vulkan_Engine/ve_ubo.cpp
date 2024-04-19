#include "ve_ubo.h"
namespace ve {
	Ubo::Ubo(VeDevice& veDevice, int maxFramesInFlight) :_veDevice{ veDevice }, maxFrames{maxFramesInFlight} {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		uniformBuffers.resize(maxFrames);
		uniformBuffersMemory.resize(maxFrames);
		uniformBufferAllocation.resize(maxFrames);
		uniformBuffersMapped.resize(maxFrames);
		for (int i = 0; i < maxFrames; i++) {

			uniformBuffersMapped[i]=(_veDevice.createBuffer(bufferSize,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT|VMA_ALLOCATION_CREATE_MAPPED_BIT,uniformBuffers[i],uniformBufferAllocation[i])).pMappedData;
		}

	}
	Ubo::~Ubo() {
		for (int i = 0; i < maxFrames; i++) {
			_veDevice.destroyBuffer(uniformBuffers[i],uniformBufferAllocation[i]);
		}
	}
	void Ubo::updateUniformBuffer(size_t currentImage,UniformBufferObject &ubo) {
		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
		_veDevice.flushAllocation(uniformBufferAllocation[currentImage],sizeof(ubo));
	}
}