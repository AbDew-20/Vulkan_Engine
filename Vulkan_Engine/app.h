#pragma once

#include "ve_window.h"
#include "ve_pipeline.h"
#include "ve_device.h"
#include "ve_swap_chain.h"
#include "ve_vertex.h"
#include <glm/glm.hpp>
#include "ve_ubo.h"
#include <glm/gtc/matrix_transform.hpp>
#include "ve_descriptor.h"


//std
#include <memory>
#include <vector>
namespace ve {
	struct MeshPushConstants {
		glm::vec2 data;

	};

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
		void createModel(std::vector<Vertex>& _vertices, std::vector<uint16_t>& _indices);
		void updateUniformBuffers(size_t currentFrame, float time);
		void createUniformBuffer();
		void recreateSwapChain();
		

		VeWindow veWindow{ WIDTH, HEIGHT, "Vulkan" };
		VeDevice veDevice{ veWindow };
		std::unique_ptr<VeSwapChain> veSwapChain;
		VeDescriptor veDescriptor{ veDevice, veSwapChain->MAX_FRAMES_IN_FLIGHT};
		std::unique_ptr<VePipeline> vePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<VertexBuffer> vertexBuffer;
		std::unique_ptr<Ubo> uniformBuffers;
		bool frameBufferResized = false;
		
		
		
	};
}