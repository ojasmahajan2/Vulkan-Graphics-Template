#pragma once

#include <string>
#include <cstdint>

struct WindowSpecification {
	std::string		title	= "Vulkan Graphics Application";
	uint32_t		width	= 854;
	uint32_t		height	= 480;
};

struct ApplicationSpecification {

	std::string instanceName = "Vulkan_Instance";

	WindowSpecification windowSpec;

};