#include "application.hpp"

void Application::run() const {
	// render loop

	float angle = 0.0f;
	float angularSpeed = 20.0f;  // ÿ����ת20��
	glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f); // ��y����ת
	bool newScene = true;
	while (!glfwWindowShouldClose(windowPtr->getWindowPtr())) {
		const auto curFrame = static_cast<float>(glfwGetTime());
		windowPtr->deltaTime = curFrame - windowPtr->lastFrame;
		windowPtr->lastFrame = curFrame;

		// get input
		Window::processInput(windowPtr->getWindowPtr());

		// rotate
		//angle += angularSpeed * windowPtr->deltaTime;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(angle), rotationAxis);
		uniformsPtr->updateModel(model);
		uniformsPtr->updateMVP(*renderPtr->getCameraPtr(),
			renderPtr->getBufferPtr()->getWidth(),
			renderPtr->getBufferPtr()->getHeight());

		/************************** render ***************************/
		if (newScene) {
			renderPtr->initFragMeshesPtr(*uniformsPtr, *shaderPtr);
			//newScene = false;
		}

		renderPtr->getBufferPtr()->clear(glm::vec3(0));
		if (renderPtr->getRasterType() == REGULAR)
			renderPtr->regularRender(*uniformsPtr, *shaderPtr);
		else if (renderPtr->getRasterType() == SCANLINE)
			renderPtr->scanLineRender(*shaderPtr, *uniformsPtr);
		else if (renderPtr->getRasterType() == NAIVEHIZ)
			renderPtr->naiveHierarchyRender(*shaderPtr, *uniformsPtr);
		else if (renderPtr->getRasterType() == OCTREEHIZ)
			renderPtr->octreeHierarchyRender(*shaderPtr, *uniformsPtr);

		// �������ز�������
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
		//break;
	}
	glfwTerminate();
}

void Application::init(size_t width, size_t height, RasterType rasterType, std::unique_ptr<Model> modelPtr) {
	auto cameraPtr = std::make_shared<Camera>(glm::vec3(0.0f, 0.f, 3.f));
	std::shared_ptr<ZBuffer> bufferPtr;
	if (rasterType == REGULAR) {
		bufferPtr = std::make_shared<RegularZBuffer>(width, height);
	}
	else if (rasterType == SCANLINE) {
		bufferPtr = std::make_shared<ScanLineZBuffer>(width, height);
	}
	else if (rasterType == NAIVEHIZ || rasterType == OCTREEHIZ) {
		bufferPtr = std::make_shared<HierarchyZBuffer>(width, height);
	}

	renderPtr = std::make_unique<Render>(std::move(modelPtr), cameraPtr, bufferPtr, rasterType);
	windowPtr = std::make_unique<Window>(width, height, "ZBufRender");
	shaderPtr = std::make_unique<Shader>(vertexShader, fragmentShader);
	uniformsPtr = std::make_unique<Uniforms>();

	Window::getContext().bufferPtr = bufferPtr.get();
	Window::getContext().uniformsPtr = uniformsPtr.get();
	Window::getContext().cameraPtr = cameraPtr.get();
}