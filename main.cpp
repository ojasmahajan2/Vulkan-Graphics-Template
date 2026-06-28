#include "app/Application.h"

int main() {
	try {
		Application app;
		app.run();
	}
	catch (const std::exception& error) {
		std::cerr << "\n[ERROR]" << error.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
