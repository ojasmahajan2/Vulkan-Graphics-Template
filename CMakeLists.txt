cmake_minimum_required(VERSION 3.20)
project(OpenGLTemplate LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch GLFW from source
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    ON
)
# Disable building GLFW docs, tests, and examples to keep builds fast
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# Fetch GLM from source
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.3
    GIT_SHALLOW    ON
)
# Disable GLM test building to save time
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glm)

# =============================================================================
# Core (Static Library)
# =============================================================================

add_library(GlCore STATIC
    core/Application.cpp
    core/Window.cpp
    core/Shader.cpp
    core/MeshBuffer.cpp
    core/gl.c             # "GLAD" to load OpenGL functions
)

target_include_directories(GlCore PUBLIC
    core/
    core/include
)

target_link_libraries(GlCore PUBLIC
    glfw                  # Links GLFW target
    glm::glm              # Links header-only GLM target
)

# =============================================================================
# App (Executable)
# =============================================================================

add_executable(app
    app/main.cpp
    app/AppLayer.cpp
)

target_link_libraries(app PRIVATE
    GlCore
)