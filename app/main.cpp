#include "../core/Application.h"
#include "AppLayer.h"

int main() {
    try {
        ApplicationSpecification spec;
        spec.title  = "OpenGL Rectangle - TEMPLATE";
        spec.width  = 1200;
        spec.height = 600;

        Application app(spec);
        
        AppLayer* appLayer = new AppLayer(app);
        app.pushLayer(appLayer);

        app.run();
        
        delete appLayer;
    }
    catch (const std::exception& error) {
        std::cerr << "\n[ERROR]: " << error.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}