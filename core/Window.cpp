#include "Window.h"

Window::Window(const std::string& title, uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {
	m_Window = SDL_CreateWindow(
		title.c_str(), m_Width, m_Height,
		SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT
	);

	if (!m_Window) {
		throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));
	}
}

Window::~Window() {
	if (m_Window) SDL_DestroyWindow(m_Window);
}

void Window::pollEvents() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {

		switch (event.type) {
			case SDL_EVENT_QUIT:
				m_ShouldClose = true;
				break;

			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
				SDL_GetWindowSizeInPixels(
					m_Window,
					reinterpret_cast<int*>(&m_Width),
					reinterpret_cast<int*>(&m_Height)
				);

				m_FramebufferResized = true;
				break;
		}
	}

}