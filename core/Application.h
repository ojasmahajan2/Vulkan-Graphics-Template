#pragma once

#include "ApplicationSpecification.h"
#include "Window.h"
#include "Layer.h"
#include "VulkanContext.h"
#include "Swapchain.h"
#include "CommandContext.h"

class Application {
public:
	Application(const ApplicationSpecification& spec);
	~Application();

	void run();
	void pushLayer(Layer* layer);

	Window&				getWindow()			{ return *m_Window; }
	VulkanContext&		getVulkanContext()	{ return *m_Ctx; }
	Swapchain&			getSwapchain()		{ return *m_Swapchain; }
	CommandContext&		getCommandContext() { return *m_CommandContext; }

	uint32_t getCurrentFrame()      const { return m_CurrentFrame; }
	uint32_t getMaxFramesInFlight() const { return MAX_FRAMES_IN_FLIGHT; }

private:
	std::unique_ptr<Window>			 m_Window;
	std::unique_ptr<VulkanContext>   m_Ctx;
	std::unique_ptr<Swapchain>       m_Swapchain;
	std::unique_ptr<CommandContext>  m_CommandContext;

	std::vector<Layer*>				 m_LayerStack;

	const uint32_t  MAX_FRAMES_IN_FLIGHT	= 2;
	uint32_t        m_CurrentFrame			= 0;

	std::chrono::steady_clock::time_point m_LastFrameTime;
	void initVulkan(const ApplicationSpecification& spec);
	
	static bool resizeEventWatcher(void* userdata, SDL_Event* event);
};
