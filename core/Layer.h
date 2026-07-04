#pragma once

#include <string>

class Layer {
public:
	Layer(const std::string& name = "Layer") : m_DebugName(name) {}

	virtual ~Layer() = default;

	virtual void onAttach()					{}
	virtual void onDetach()					{}
	virtual void onUpdate(float deltaTime)	{}
	virtual void onRender()					{}

	const std::string getName() const { return m_DebugName; }

private:
	std::string m_DebugName;
};