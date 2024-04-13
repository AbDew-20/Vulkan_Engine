
#include "app.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include "ve_vertex.h"
	int main() {
		std::vector<ve::Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}}
		};

		std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };


		ve::app app(vertices,indices);

		try {
			app.run();
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << '\n';
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}
