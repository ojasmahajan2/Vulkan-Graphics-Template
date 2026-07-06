#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <cstdlib>
#include <chrono>

struct glfwErrorGet {
    glfwErrorGet() {
        code = glfwGetError(&description);
    }

    const char* description = nullptr;
    int code = 0;

    std::string glfwErrorMsg(const std::string& msg) const {
        std::string formatMsg = msg + "\nError Code: " + std::to_string(code);

        if (!description) return formatMsg;

        return formatMsg + "\n" + description;
    }
};