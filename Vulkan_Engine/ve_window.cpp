#include "ve_window.h"

#include <stdexcept>
namespace ve {
	VeWindow::VeWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		intWindow();
	}

	VeWindow::~VeWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VeWindow::intWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

		window = glfwCreateWindow(width,height,windowName.c_str(),nullptr,nullptr );
	}

	void VeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
	}

}