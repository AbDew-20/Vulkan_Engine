#include "ve_Light.h"

namespace ve{
	VeLight::VeLight(VeDevice& device,LightUBO& lightData, int MAX_FRAMES_IN_FLIGHT) :veDevice{ device }, maxFrames{MAX_FRAMES_IN_FLIGHT} {
		
		createLightBuffer(lightData);
	}

	VeLight::~VeLight() {
		veDevice.destroyBuffer(lightBuffer, lightBufferAllocation);
	}

	void VeLight::createLightBuffer(LightUBO& lightData) {
		VkDeviceSize bufferSize = sizeof(LightUBO);
		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferAllocation;
		(void)veDevice.createBuffer(bufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,stagingBuffer,stagingBufferAllocation);
		veDevice.fillBuffer(stagingBufferAllocation,0,bufferSize,&lightData);
		(void)veDevice.createBuffer(bufferSize,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT,0,lightBuffer,lightBufferAllocation);
		veDevice.copyBuffer(stagingBuffer,lightBuffer,bufferSize);
		veDevice.destroyBuffer(stagingBuffer,stagingBufferAllocation);
	
	}


}