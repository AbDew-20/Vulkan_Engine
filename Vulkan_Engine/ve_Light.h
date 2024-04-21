#pragma once
#include <glm/glm.hpp>
#include "ve_device.h"
#include <vector>
#include <array>

namespace ve {
	struct LightUBO {
		alignas(16)glm::vec3 lightPos;
		alignas(16)glm::vec3 lightColor;
		alignas(4)float lightIntensity;
	};

	class VeLight {
	public:
		VeLight(VeDevice& device,LightUBO& lightData ,int MAX_FRAMES_IN_FLIGHT);
		~VeLight();
		inline VkBuffer getBuffer() { return lightBuffer; }
	private:

		void createLightBuffer(LightUBO& lightData);
		VkBuffer lightBuffer;
		VmaAllocation lightBufferAllocation;
		VeDevice& veDevice;
		int maxFrames;

		
	
	};


}
