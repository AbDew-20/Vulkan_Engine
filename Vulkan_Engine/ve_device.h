#pragma once
#include <vk_mem_alloc.h>
#include "ve_window.h"
// std lib headers
#include <string>
#include <vector>

namespace ve {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  uint32_t graphicsFamily;
  uint32_t presentFamily;
  bool graphicsFamilyHasValue = false;
  bool presentFamilyHasValue = false;
  bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

class VeDevice {
 public:
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif

  VeDevice(VeWindow &window);
  ~VeDevice();

  // Not copyable or movable
  VeDevice(const VeDevice &) = delete;
  VeDevice& operator=(const VeDevice&) = delete;

  VkCommandPool getCommandPool() { return commandPool; }
  VkDevice device() { return device_; }
  VkSurfaceKHR surface() { return surface_; }
  VkQueue graphicsQueue() { return graphicsQueue_; }
  VkQueue presentQueue() { return presentQueue_; }

  SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }
  VkFormat findSupportedFormat(
      const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
  VkPhysicalDeviceLimits getDeviceLimits();
  // Buffer Helper Functions
  
  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void copyBufferToImage(
      VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
  VmaAllocationInfo createBuffer(VkDeviceSize size,VkBufferUsageFlags usage,VmaAllocationCreateFlags prcreationFlags,VkBuffer& buffer,VmaAllocation& bufferMemory);
  void destroyBuffer(VkBuffer &buffer, VmaAllocation &bufferMemory);
  void fillBuffer(VmaAllocation &bufferMemory,VkDeviceSize offset,VkDeviceSize size, const void* _src);
  void flushAllocation(VmaAllocation& bufferMemory, VkDeviceSize size);

  //Image helper functions
  void createImageWithInfo(const VkImageCreateInfo& imageInfo,
      VmaAllocationCreateFlags creationFlags,
      VkImage& image,
      VmaAllocation& imageMemory);
  void destroyImage(VkImage& image, VmaAllocation& imageAllocation);


  VkPhysicalDeviceProperties properties;
  
  void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,uint32_t mipLevels);
  void generateMipmaps(VkImage image,VkFormat imageFormat,int32_t texWidth,int32_t texHeight,uint32_t mipLevels);

 private:
  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createCommandPool();
  void createAllocator();
  

  // helper functions
  bool isDeviceSuitable(VkPhysicalDevice device);
  std::vector<const char *> getRequiredExtensions();
  bool checkValidationLayerSupport();
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
  void hasGflwRequiredInstanceExtensions();
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VeWindow &window;
  VkCommandPool commandPool;

  VkDevice device_;
  VkSurfaceKHR surface_;
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;
  VmaAllocator allocator;

  const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
  const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}  // namespace lve