#pragma once
#include <stb_image.h>
#include "ve_device.h"


namespace ve{

	class VeTexture{

	public:
		VeTexture(VeDevice& device,const char *texPath);
		~VeTexture();

		VeTexture(const VeTexture&) = delete;
		VeTexture operator=(const VeTexture&) = delete;
		inline VkImageView getImageView() { return texImageView; }
		inline VkSampler getSampler() { return texSampler; }
	

	private:
		void createTextureImage();
		void createTextureImageView();
		void createTextureSampler();
		const char* filePath;
		VeDevice& veDevice;
		VkImage texImage;
		VkDeviceMemory texImageMemory;
		VkImageView texImageView;
		VkSampler texSampler;
		uint32_t mipLevels;
	};

}
