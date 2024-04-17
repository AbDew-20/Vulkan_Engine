#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
namespace ve {
	class VeWindow{
	public:
		VeWindow(int w, int h, std::string name);
		~VeWindow();

		VeWindow(const VeWindow &) = delete;
		VeWindow &operator = (const VeWindow&)= delete;
		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
		void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
		inline bool wasWindowResized() { return frameBufferResized; }
		void resetWindowResizeFlag() { frameBufferResized = false; }
		void getFrameBufferSize(int *width,int *height);
	private:
		void intWindow();
		int width;
		int height;
		std::string windowName;
		GLFWwindow* window;
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		bool frameBufferResized = false;

	};
}