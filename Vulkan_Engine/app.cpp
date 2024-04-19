#include "app.h"
#include <stdexcept>
#include <array>
#include <chrono>
namespace ve{
	app::app(std::vector<Vertex> &_vertices, std::vector<uint16_t> &_indices) {
		createPipelineLayout();
		recreateSwapChain();
		createModel(_vertices, _indices);
		loadTextures();
		createUniformBuffer();
		createCommandBuffers();
		veDescriptor.createDescriptorSets(uniformBuffers, mulTextures);
		
	}
	app::~app() {
		vkDestroyPipelineLayout(veDevice.device(), pipelineLayout, nullptr);

	}
	void app::run() {
		while (!veWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}
		vkDeviceWaitIdle(veDevice.device());
	}

	void app::createPipelineLayout() {
		VkPushConstantRange pushConstants{};
		pushConstants.offset = 0;
		pushConstants.size = sizeof(MeshPushConstants);
		pushConstants.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = veDescriptor.getDescriptorLayout();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstants;
		if (vkCreatePipelineLayout(veDevice.device(),&pipelineLayoutInfo,nullptr,&pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void app::createPipeline() {
		auto pipelineConfig = VePipeline::defaultPipelineConfigInfo(veSwapChain->width(), veSwapChain->height());
		pipelineConfig.renderPass = veSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		vePipeline = std::make_unique<VePipeline>(veDevice, "Shaders/simple_shader.vert.spv", "Shaders/simple_shader.frag.spv",pipelineConfig);

	}
	void app::createCommandBuffers() {
		commandBuffers.resize(veSwapChain->imageCount());
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

		
		//Updating command buffer
		uint32_t imageIndex;
		auto result= veSwapChain->acquireNextImage(&imageIndex);
		if (result==VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
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
		renderPassInfo.renderPass = veSwapChain->getRenderPass();
		renderPassInfo.framebuffer = veSwapChain->getFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = veSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();
		//PushConstants
		MeshPushConstants pushConstants{};
		
		pushConstants.diffuseColor = {1.0f,1.0f,1.0f,1.0f};
		pushConstants.lightDirection = {0.0f,1.0f,1.0f};
		vkCmdPushConstants(commandBuffers[imageIndex],pipelineLayout,VK_SHADER_STAGE_FRAGMENT_BIT,0,sizeof(MeshPushConstants),&pushConstants);

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vePipeline->bind(commandBuffers[imageIndex]);


		VkBuffer vertexBuffers[] = { vertexBuffer->getVertBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

		VkBuffer indexBuffer = vertexBuffer->getIndexBuffer();
		vkCmdBindIndexBuffer(commandBuffers[imageIndex], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(commandBuffers[imageIndex],VK_PIPELINE_BIND_POINT_GRAPHICS,pipelineLayout,0,1,veDescriptor.getDescriptorSets()+veSwapChain->getCurrentFrame(), 0, nullptr);
		vkCmdDrawIndexed(commandBuffers[imageIndex], static_cast<uint32_t>(vertexBuffer->getVerticesNum()), 2, 0, 0, 1);
		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}

		updateUniformBuffers(veSwapChain->getCurrentFrame(), time);
		result = veSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result==VK_ERROR_OUT_OF_DATE_KHR||result==VK_SUBOPTIMAL_KHR||veWindow.wasWindowResized()) {
			veWindow.resetWindowResizeFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swapchain image");
		}
	}
	
	void app::createModel(std::vector<Vertex>& _vertices, std::vector<uint16_t>& _indices) {
		vertexBuffer = std::make_unique<VertexBuffer>(veDevice, _vertices, _indices);
	}
	void app::updateUniformBuffers(size_t currentFrame, float time) {
		UniformBufferObject ubo{};
		ubo.model[0] = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.0f, 0.0f)), sin(time+1.5f) * glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.model[1] = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, 0.0f, 0.0f)), sin(time) * glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), veSwapChain->extentAspectRatio(), 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		uniformBuffers->updateUniformBuffer(currentFrame,ubo);
	}
	void app::createUniformBuffer() {
		uniformBuffers = std::make_unique<Ubo>(veDevice,veSwapChain->MAX_FRAMES_IN_FLIGHT);
	}
	void app::recreateSwapChain() {
		int width = 0;
		int height = 0;
		veWindow.getFrameBufferSize(&width, &height);
		while (width == 0 || height == 0) {
			veWindow.getFrameBufferSize(&width,&height);
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(veDevice.device());
		veSwapChain.reset();
		veSwapChain = std::make_unique<VeSwapChain>(veDevice, veWindow.getExtent());
		createPipeline();
	}
	void app::loadTextures() {
		mulTextures.push_back(std::make_unique<VeTexture>(veDevice, "Textures/statue1.jpg"));
		mulTextures.push_back(std::make_unique<VeTexture>(veDevice, "Textures/stone01.tga"));
		textures = std::make_unique<VeTexture>(veDevice,"Textures/stone01.tga");
	
	}
	
}