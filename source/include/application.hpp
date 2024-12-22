#pragma once

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

#include "render.hpp"
#include "window.hpp"

class Application {
public:
	Application() = default;

	~Application() = default;

	Application(size_t width, size_t height, RasterType rasterType) { init(width, height, rasterType); }

	void run() const;

private:
	std::unique_ptr<Window> windowPtr;
	std::unique_ptr<Shader> shaderPtr;
	std::shared_ptr<Uniforms> uniformsPtr;
	std::unique_ptr<Render> renderPtr;

	void init(size_t width, size_t height, RasterType rasterType);
};