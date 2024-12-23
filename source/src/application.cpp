#include "application.hpp"

void Application::run() const {
	// render loop

	float angle = 0.0f;
	float angularSpeed = 20.0f;  // 每秒旋转20度
	glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

	while (!glfwWindowShouldClose(windowPtr->getWindowPtr())) {
		auto start = std::chrono::high_resolution_clock::now();

		// set time
		renderPtr->getBufferPtr()->clear(glm::vec3(0));
		const auto curFrame = static_cast<float>(glfwGetTime());
		windowPtr->deltaTime = curFrame - windowPtr->lastFrame;
		windowPtr->lastFrame = curFrame;

		// get input
		Window::processInput(windowPtr->getWindowPtr());

		// rotate
		angle += angularSpeed * windowPtr->deltaTime;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(angle), rotationAxis);
		uniformsPtr->updateModel(model);
		uniformsPtr->updateMVP(*renderPtr->getCameraPtr(),
			renderPtr->getBufferPtr()->getWidth(),
			renderPtr->getBufferPtr()->getHeight());

		// render type
		if (renderPtr->getRasterType() == REGULAR)
			renderPtr->regularRender(*uniformsPtr, *shaderPtr, false);
		else if (renderPtr->getRasterType() == SCANLINE)
			renderPtr->scanLineRender(*shaderPtr, *uniformsPtr);
		else if (renderPtr->getRasterType() == NAIVE)
			renderPtr->naiveHierarchyRender(*shaderPtr, *uniformsPtr, true);

		// 设置像素操作参数
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glDrawPixels(static_cast<GLsizei>(renderPtr->getBufferPtr()->getWidth()),
			static_cast<GLsizei>(renderPtr->getBufferPtr()->getHeight()),
			GL_RGB,
			GL_UNSIGNED_BYTE,
			renderPtr->getBufferPtr()->getPixelPtr()->data());

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		// glfwSwapInterval(1);
		glfwSwapBuffers(windowPtr->getWindowPtr());

		glfwPollEvents();

		auto end = std::chrono::high_resolution_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "Execution time: " << duration << " ms" << std::endl;
	}
	glfwTerminate();
}

void Application::init(size_t width, size_t height, RasterType rasterType) {
	auto modelPtr = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "armadillo.obj");  // armadillo
	auto cameraPtr = std::make_shared<Camera>(glm::vec3(0.0f, 0.f, 6.0f));
	std::shared_ptr<ZBuffer> bufferPtr;
	if (rasterType == REGULAR) {
		bufferPtr = std::make_shared<RegularZBuffer>(width, height);
	}
	else if (rasterType == SCANLINE) {
		bufferPtr = std::make_shared<ScanLineZBuffer>(width, height);
	}
	else if (rasterType == NAIVE) {
		bufferPtr = std::make_shared<NaiveHierarchyZBuffer>(width, height);
	}

	renderPtr = std::make_unique<Render>(std::move(modelPtr), cameraPtr, bufferPtr, rasterType);
	windowPtr = std::make_unique<Window>(width, height, "ZBufRender");
	shaderPtr = std::make_unique<Shader>(vertexShader, fragmentShader);
	uniformsPtr = std::make_unique<Uniforms>();

	Window::getContext().bufferPtr = bufferPtr.get();
	Window::getContext().uniformsPtr = uniformsPtr.get();
	Window::getContext().cameraPtr = cameraPtr.get();
}