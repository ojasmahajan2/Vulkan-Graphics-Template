#pragma once

#include "headers.h"

struct SDL_InitRAII {
	SDL_InitRAII(SDL_InitFlags flag) {
		if (!SDL_Init(flag)) {
			throw std::runtime_error("Failed to initiate SDL3: " + std::string(SDL_GetError()));
		}
	}

	~SDL_InitRAII() {
		SDL_Quit();
		std::cout << "SDL3 Utility cleared" << std::endl;
	}

	SDL_InitRAII(const SDL_InitRAII&) = delete;
	SDL_InitRAII& operator=(const SDL_InitRAII&) = delete;
};

class Window {
public:
	Window(const std::string& title, const uint32_t width, const uint32_t height);
	~Window();

	SDL_Window* getSDLWindow() const { return m_Window; }
	bool		shouldClose()  const { return m_ShouldClose; }

	void pollEvents();

	uint32_t getWidth()		const { return m_Width; }
	uint32_t getHeight()	const { return m_Height; }

	void setFramebufferResized(bool resized)	{ m_FramebufferResized = resized; }
	bool wasFramebufferResized()				{ return m_FramebufferResized; }
	void resetFramebufferResized()				{ m_FramebufferResized = false; }

private:
	SDL_InitRAII m_SDLInit{ SDL_INIT_VIDEO };

	SDL_Window* m_Window				= nullptr;
	uint32_t    m_Width;
	uint32_t    m_Height;
	bool        m_ShouldClose			= false;
	bool        m_FramebufferResized	= false;
};