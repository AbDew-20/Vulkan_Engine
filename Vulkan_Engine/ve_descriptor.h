#pragma once
#include "ve_device.h"
#include "ve_ubo.h"
#include "ve_texture.h"
#include <memory>
#include <array>

namespace ve {
	class VeDescriptor {
	public:
		VeDescriptor(VeDevice& device, size_t max_frames_in_flight);
		~VeDescriptor();

		VeDescriptor(const VeDescriptor&) = delete;
		VeDescriptor& operator=(const VeDescriptor&) = delete;

		inline VkDescriptorSet *getDescriptorSets() { return &descriptorSets[0]; }
		void createDescriptorSets(std::unique_ptr<Ubo> &uniformBuffers, std::vector<std::unique_ptr<VeTexture>> &texture);
		inline VkDescriptorSetLayout *getDescriptorLayout() { return &descriptorSetLayout; }
	private:
		void createDescriptorPool();
		void createDescriptorSetLayout();
		

		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<VkDescriptorSet> descriptorSets;
		VeDevice& veDevice;
		const size_t MAX_FRAMES_IN_FLIGHT;


	};
	
}
