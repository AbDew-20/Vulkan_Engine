#pragma once

#include "ve_window.h"
#include "ve_pipeline.h"
#include "ve_device.h"
#include "ve_swap_chain.h"
#include "ve_vertex.h"


//std
#include <memory>
#include <vector>
namespace ve {
	class app {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		app(std::vector<Vertex> &_vertices, std::vector<uint16_t> &_indices);
		~app();

		app(const app&) = delete;
		app& operator = (const app&) = delete;
		void run();
	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();
		void createDescriptorSetLayout();
		void createModel(std::vector<Vertex>& _vertices, std::vector<uint16_t>& _indices);

		VeWindow veWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		VeDevice veDevice{ veWindow };
		VeSwapChain veSwapChain{veDevice, veWindow.getExtent()};
		std::unique_ptr<VePipeline> vePipeline;
		VkPipelineLayout pipelineLayout;
		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<VertexBuffer> vertexBuffer;
		
		
	};
}