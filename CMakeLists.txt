cmake_minimum_required(VERSION 3.20)
project(VulkanTemplate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================================================
# Core (Static Library)
# =========================================================

find_package(Vulkan REQUIRED)

add_library(VkCore STATIC
    core/Application.cpp
    core/VulkanContext.cpp
    core/Swapchain.cpp
    core/Pipeline.cpp
    core/MeshBuffer.cpp
    core/CommandContext.cpp
    core/Window.cpp
)

target_include_directories(VkCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    $ENV{VULKAN_SDK}/Include
    $ENV{VULKAN_SDK}/Include/slang
)

target_link_directories(VkCore PUBLIC
    $ENV{VULKAN_SDK}/Lib
)

target_link_libraries(VkCore PUBLIC
    Vulkan::Vulkan
        sdl3
        slang
)

# =========================================================
# App (Executable)
# =========================================================

add_executable(app
    app/main.cpp  
    app/AppLayer.cpp 
)

target_link_libraries(app PRIVATE
    VkCore
)

add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$ENV{VULKAN_SDK}/Bin/SDL3.dll"
        "$ENV{VULKAN_SDK}/Bin/slang.dll"
        "$ENV{VULKAN_SDK}/Bin/slang-compiler.dll"
        $<TARGET_FILE_DIR:app>
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:app>/shaders"
)