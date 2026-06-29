cmake_minimum_required(VERSION 3.20)
project(VulkanTEMPLATE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(VulkanTEMPLATE
    main.cpp
        app/Application.cpp
        core/VulkanContext.cpp
        core/Swapchain.cpp
        core/Pipeline.cpp
        core/MeshBuffer.cpp
        core/CommandContext.cpp
)

find_package(Vulkan REQUIRED)

target_include_directories(VulkanTEMPLATE PRIVATE
    ${CMAKE_SOURCE_DIR}/
        ${CMAKE_SOURCE_DIR}/app
        ${CMAKE_SOURCE_DIR}/core
        $ENV{VULKAN_SDK}/Include
        $ENV{VULKAN_SDK}/Include/slang
)

target_link_directories(VulkanTEMPLATE PRIVATE
    $ENV{VULKAN_SDK}/Lib
)

target_link_libraries(VulkanTEMPLATE PRIVATE
    Vulkan::Vulkan
        sdl3
        slang
)

add_custom_command(TARGET VulkanTEMPLATE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$ENV{VULKAN_SDK}/Bin/SDL3.dll"
        "$ENV{VULKAN_SDK}/Bin/slang.dll"
        "$ENV{VULKAN_SDK}/Bin/slang-compiler.dll"
        $<TARGET_FILE_DIR:VulkanTEMPLATE>
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:VulkanTEMPLATE>/shaders"
)