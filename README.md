# OpenGL Graphics Template

This project is a streamlined graphics framework built on OpenGL 3.3 Core. It demonstrates a clean, layered application design that separates core rendering logic from application-level logic.

## Project Structure

The codebase is organized into two primary components:
* **GlCore.lib:** The core engine static library. It handles window creation, rendering contexts, shader management, and the main event loop.
* **App:** The executable that links against GlCore.lib. It contains the application-specific logic, such as updating matrices and commanding draws.

## Architectural Design

This project adopts a "polylithic" layered architecture. It was specifically designed to mirror the structural patterns of the Vulkan-Graphics-Template (which utilized a similar VkCore.lib). The goal is to provide a clean, object-oriented abstraction without introducing unnecessary complexity. 

Key design features include:
* **Layer System:** Application behavior is implemented through a `Layer` interface, allowing the core to remain completely agnostic of the app's specific rendering logic.
* **Real-time Event Processing:** The main loop and window callbacks are tightly integrated to ensure smooth transitions and aspect-ratio retention during window resizing.

## Compatibility and Portability

The framework is highly portable and compatible across a wide range of hardware:
* Targets OpenGL 3.3 Core Profile, which is supported by nearly all modern and legacy graphics drivers.
* Uses an OpenGL 4.6 loader generated via the GLAD2 web service, allowing access to newer API features if needed while maintaining 3.3 target guidelines.
* Utilizes a cross-platform CMake build system.
* Leverages standard cross-platform libraries (GLFW for window management and GLM for mathematics).
