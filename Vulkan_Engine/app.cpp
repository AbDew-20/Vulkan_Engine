#include "app.h"
#include <stdexcept>
#include <array>
#include <chrono>
namespace ve{
	app::app(std::vector<Vertex> &_vertices, std::vector<uint16_t> &_indices) {
		createPipelineLayout();
		createPipeline();
		createModel(_vertices, _indices);
		createCommandBuffers();
	}
	app::~app() {
		vkDestroyPipelineLayout(veDevice.device(), pipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(veDevice.device(), descriptorSetLayout, nullptr);
	}
	void app::run() {
		while (!veWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}
		vkDeviceWaitIdle(veDevice.device());
	}

	void app::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(veDevice.device(),&pipelineLayoutInfo,nullptr,&pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void app::createPipeline() {
		auto pipelineConfig = VePipeline::defaultPipelineConfigInfo(veSwapChain.width(), veSwapChain.height());
		pipelineConfig.renderPass = veSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		vePipeline = std::make_unique<VePipeline>(veDevice, "Shaders/simple_shader.vert.spv", "Shaders/simple_shader.frag.spv",pipelineConfig);

	}
	void app::createCommandBuffers() {
		commandBuffers.resize(veSwapChain.imageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = veDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(veDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command buffer");
		}
	}
	void app::drawFrame() {
		static auto startClock = std::chrono::high_resolution_clock::now();
		auto nowClock = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(startClock - nowClock).count();

		uint32_t imageIndex;
		auto result= veSwapChain.acquireNextImage(&imageIndex);
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to aquire swapchain image");
		}
		if (vkResetCommandBuffer(commandBuffers[imageIndex], 0) != VK_SUCCESS) {
			throw std::runtime_error("failed to reset command buffer");
		}
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = veSwapChain.getRenderPass();
		renderPassInfo.framebuffer = veSwapChain.getFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = veSwapChain.getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vePipeline->bind(commandBuffers[imageIndex]);


		VkBuffer vertexBuffers[] = { vertexBuffer->getVertBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

		VkBuffer indexBuffer = vertexBuffer->getIndexBuffer();
		vkCmdBindIndexBuffer(commandBuffers[imageIndex], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdDrawIndexed(commandBuffers[imageIndex], static_cast<uint32_t>(vertexBuffer->getVerticesNum()), 1, 0, 0, 0);
		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}


		result = veSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swapchain image");
		}
	}
	void app::createDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;


		if (vkCreateDescriptorSetLayout(veDevice.device(), &layoutInfo,nullptr,&descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout");
		}

	}
	void app::createModel(std::vector<Vertex>& _vertices, std::vector<uint16_t>& _indices) {
		vertexBuffer = std::make_unique<VertexBuffer>(veDevice, _vertices, _indices);
	}
}