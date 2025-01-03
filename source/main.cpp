#include "application.hpp"

int main() {
	/****************************** choose model to render *******************************/
	//auto modelPtr = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "suzanne.obj");
	//auto modelPtr = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "bbunny.obj");
	auto modelPtr = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "armadillo.obj");

	/****************************** choose render type *******************************/
	//const Application app{ 800, 600, REGULAR, std::move(modelPtr) };
	//const Application app{ 800, 600, SCANLINE, std::move(modelPtr) };
	//const Application app{ 800, 600, NAIVE, std::move(modelPtr) };
	const Application app{ 800, 600, OCTREE, std::move(modelPtr) };

	app.run();
	return 0;
}