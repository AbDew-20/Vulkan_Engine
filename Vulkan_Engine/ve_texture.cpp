#define STB_IMAGE_IMPLEMENTATION
#include "ve_texture.h"
#include <cmath>
#include <stdexcept>

namespace ve {

	VeTexture::VeTexture(VeDevice& device,const char* texPath) :filePath{ texPath }, veDevice{device} {
		createTextureImage();
		createTextureImageView();
		createTextureSampler();
	}

	VeTexture::~VeTexture() {
		vkDestroySampler(veDevice.device(), texSampler, nullptr);
		vkDestroyImageView(veDevice.device(),texImageView,nullptr);

		veDevice.destroyImage(texImage,texImageAllocation);
	}

	void VeTexture::createTextureImage() {
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(filePath,&texWidth,&texHeight,&texChannels,STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;
		if(!pixels){
			throw std::runtime_error("failed to load image");
		}
		mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

		VkBuffer stagingBuffer;

		VmaAllocation staginBufferAllocation;
		(void)veDevice.createBuffer(imageSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,stagingBuffer,staginBufferAllocation);
		veDevice.fillBuffer(staginBufferAllocation,0,imageSize,pixels);
		stbi_image_free(pixels);

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(texWidth);
		imageInfo.extent.height = static_cast<uint32_t>(texHeight);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;//optional
		
		veDevice.createImageWithInfo(imageInfo,0,texImage,texImageAllocation);
		veDevice.transitionImageLayout(texImage,VK_FORMAT_R8G8B8A8_SRGB,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,mipLevels);
		veDevice.copyBufferToImage(stagingBuffer,texImage,static_cast<uint32_t>(texWidth),static_cast<uint32_t>(texHeight),1);
		veDevice.generateMipmaps(texImage,VK_FORMAT_R8G8B8A8_SRGB,texWidth,texHeight,mipLevels);

		veDevice.destroyBuffer(stagingBuffer,staginBufferAllocation);
	}

	void VeTexture::createTextureImageView() {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = texImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(veDevice.device(),&viewInfo,nullptr,&texImageView)!=VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view");
		}
	}
	void VeTexture::createTextureSampler() {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = veDevice.getDeviceLimits().maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(mipLevels);

		if (vkCreateSampler(veDevice.device(), &samplerInfo,nullptr,&texSampler)!=VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler");
		}
		
	}
	

}