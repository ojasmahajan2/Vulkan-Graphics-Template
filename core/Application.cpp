#include "Application.h"

Application::Application(const ApplicationSpecification& spec) {
	m_Window = std::make_unique<Window>(
		spec.windowSpec.title,
		spec.windowSpec.width,
		spec.windowSpec.height
	);

	initVulkan(spec);

	SDL_AddEventWatch(resizeEventWatcher, this);

	m_LastFrameTime = std::chrono::steady_clock::now();
}

Application::~Application() {
	SDL_RemoveEventWatch(resizeEventWatcher, this);

	if (!m_LayerStack.empty()) {
		m_Ctx->device.waitIdle();
		for (Layer* layer : m_LayerStack) {
			layer->onDetach();
		}
	}
}

void Application::pushLayer(Layer* layer) {
	m_LayerStack.push_back(layer);
	layer->onAttach();
}

bool Application::resizeEventWatcher(void* userdata, SDL_Event* event) {
	if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
		auto* app = static_cast<Application*>(userdata);
		
		app->m_Window->setFramebufferResized(true);

		int w, h;
		SDL_GetWindowSizeInPixels(app->m_Window->getSDLWindow(), &w, &h);
		if (w == 0 || h == 0) {
			return true;
		}

		auto now = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration<float>(now - app->m_LastFrameTime).count();
		app->m_LastFrameTime = now;

		for (Layer* layer : app->m_LayerStack) {
			layer->onUpdate(deltaTime);
		}

		for (Layer* layer : app->m_LayerStack) {
			layer->onRender();
		}

		app->m_CurrentFrame = (app->m_CurrentFrame + 1) % app->MAX_FRAMES_IN_FLIGHT;
	}
	return true;
}

void Application::run() {
	std::cout << std::endl;

	while (!m_Window->shouldClose()) {
		m_Window->pollEvents();

		if (m_Window->getWidth() == 0 || m_Window->getHeight() == 0) {
			continue;
		}

		auto now = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration<float>(now - m_LastFrameTime).count();

		m_LastFrameTime = now;

		for (Layer* layer : m_LayerStack) {
			layer->onUpdate(deltaTime);
		}

		for (Layer* layer : m_LayerStack) {
			layer->onRender();
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	std::cout << std::endl;

	m_Ctx->device.waitIdle();

	for (Layer* layer : m_LayerStack) {
		layer->onDetach();
	}
	m_LayerStack.clear();
}

void Application::initVulkan(const ApplicationSpecification& spec) {
	m_Ctx				= std::make_unique<VulkanContext>(m_Window->getSDLWindow(), spec.instanceName);
	m_Swapchain			= std::make_unique<Swapchain>(*m_Ctx, m_Window->getWidth(), m_Window->getHeight());
	m_CommandContext	= std::make_unique<CommandContext>(*m_Ctx, MAX_FRAMES_IN_FLIGHT, m_Swapchain->images.size());
}