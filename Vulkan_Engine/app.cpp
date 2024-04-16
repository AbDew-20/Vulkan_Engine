#include "app.h"
#include <stdexcept>
#include <array>
#include <chrono>
namespace ve{
	app::app(std::vector<Vertex> &_vertices, std::vector<uint16_t> &_indices) {
		createDescriptorPool();
		createDescriptorSetLayout();
		createPipelineLayout();
		createPipeline();
		createModel(_vertices, _indices);
		createUniformBuffer();
		createCommandBuffers();
		createDescriptorSets();
	}
	app::~app() {
		vkDestroyDescriptorPool(veDevice.device(),descriptorPool,nullptr);
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
		VkPushConstantRange pushConstants{};
		pushConstants.offset = 0;
		pushConstants.size = sizeof(MeshPushConstants);
		pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstants;
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

		
		//Updating command buffer
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
		//PushConstants
		MeshPushConstants pushConstants{};
		float amplitude = 0.2f;
		pushConstants.data = { amplitude * sin(time),amplitude * cos(time) };
		vkCmdPushConstants(commandBuffers[imageIndex],pipelineLayout,VK_SHADER_STAGE_VERTEX_BIT,0,sizeof(MeshPushConstants),&pushConstants);

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vePipeline->bind(commandBuffers[imageIndex]);


		VkBuffer vertexBuffers[] = { vertexBuffer->getVertBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

		VkBuffer indexBuffer = vertexBuffer->getIndexBuffer();
		vkCmdBindIndexBuffer(commandBuffers[imageIndex], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(commandBuffers[imageIndex],VK_PIPELINE_BIND_POINT_GRAPHICS,pipelineLayout,0,1,&descriptorSets[veSwapChain.getCurrentFrame()], 0, nullptr);

		vkCmdDrawIndexed(commandBuffers[imageIndex], static_cast<uint32_t>(vertexBuffer->getVerticesNum()), 1, 0, 0, 0);
		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}

		updateUniformBuffers(veSwapChain.getCurrentFrame(), time);
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
	void app::updateUniformBuffers(size_t currentFrame, float time) {
		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,0.0f)), time * glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), veSwapChain.extentAspectRatio(), 0.1f, 10.0f);

		uniformBuffers->updateUniformBuffer(currentFrame,ubo);
	}
	void app::createUniformBuffer() {
		uniformBuffers = std::make_unique<Ubo>(veDevice,veSwapChain.MAX_FRAMES_IN_FLIGHT);
	}
	void app::createDescriptorPool() {
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(veSwapChain.MAX_FRAMES_IN_FLIGHT);
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets= static_cast<uint32_t>(veSwapChain.MAX_FRAMES_IN_FLIGHT);

		if (vkCreateDescriptorPool(veDevice.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool");
		}

	}

	void app::createDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts(veSwapChain.MAX_FRAMES_IN_FLIGHT,descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(veSwapChain.MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();
		descriptorSets.resize(veSwapChain.MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(veDevice.device(),&allocInfo,descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets");
		}

		for (size_t i = 0; i < static_cast<uint32_t>(veSwapChain.MAX_FRAMES_IN_FLIGHT); i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers->getBuffer(i);
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;
			vkUpdateDescriptorSets(veDevice.device(),1,&descriptorWrite,0,nullptr);
		}
	}
}