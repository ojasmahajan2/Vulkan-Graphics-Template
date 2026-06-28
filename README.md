# Vulkan Engine Template

A modular Vulkan starting project designed to serve as a clean foundation for graphics applications. This template simplifies common Vulkan setup by using Resource Acquisition Is Initialization (RAII) principles, dynamic rendering, and runtime shader compilation.

## Key Features

* **Dynamic Rendering:** Uses Vulkan 1.3 dynamic rendering (`vk::RenderingInfo`), removing the need for legacy `vk::RenderPass` and `vk::Framebuffer` objects. This simplifies the render loop and makes the pipeline easier to modify.
* **RAII Memory Management:** Built with `vk::raii` wrappers, standard smart pointers (`std::unique_ptr`), and Vulkan Memory Allocator (VMA). Resources are automatically cleaned up when they go out of scope, avoiding memory leaks and reducing manual cleanup code.
* **Runtime Shader Compilation:** Integrates the Slang shading language to compile shaders into SPIR-V at runtime. You do not need to pre-compile shaders using offline scripts.
* **Dynamic Mesh Loading:** Includes a `MeshBuffer` class that handles index offsets automatically. This allows you to load and batch multiple independent shapes into a single unified vertex and index buffer on the GPU.
* **Self-Documenting Code Structure:** The codebase is organized cleanly to be readable and understandable without relying on extensive code comments.

## Refactoring from a Monolith

This template was created by restructuring a traditional single-file Vulkan monolith into a more scalable design. The main differences include:

* **Modular Architecture:** The code is divided into `app/` and `core/` directories. The high-level application logic is kept completely separate from the low-level Vulkan wrapper classes (`VulkanContext`, `Swapchain`, `Pipeline`, `CommandContext`, and `MeshBuffer`).
* **Constructor-Based Setup:** Manual `init()` and `cleanup()` functions were replaced. The project relies entirely on constructors for setup and C++ scoping rules for safe destruction, improving stability.
* **Simplified Feature Set:** Extra features from the original monolith (such as debug modes, multi-pipeline toggles, and rotation logic) were removed to keep this template as minimal and focused as possible for new projects.

## Requirements

* C++20 Compiler
* Vulkan SDK 1.3 or newer
* SDL3
* Vulkan Memory Allocator (VMA)
* Slang Compiler
