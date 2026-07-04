#include "Application.h"
#include "AppLayer.h"

int main() {
    try {
        ApplicationSpecification spec;
        spec.windowSpec.title  = "Vulkan Rectangle - TEMPLATE";
        spec.windowSpec.width  = 1200;
        spec.windowSpec.height = 600;

        Application app(spec);

        AppLayer appLayer(app);
        app.pushLayer(&appLayer);

        app.run();
    }
    catch (const std::exception& error) {
        std::cerr << "\n[ERROR] " << error.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
