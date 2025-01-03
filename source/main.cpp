#include "application.hpp"

int main() {
	/****************************** choose model to render *******************************/
	auto start = std::chrono::high_resolution_clock::now();
	//auto modelPtr = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "suzanne_1k.obj");
	//auto modelPtr = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "teapot_6k.obj");
	auto modelPtr = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "bunny_69k.obj");
	//auto modelPtr = std::make_unique<Model>(R"(D:\code\ZBufRender\asserts)", "armadillo_212k.obj");
	auto end = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "load model time: " << duration << " ms" << std::endl;

	/****************************** choose render type *******************************/
	//const Application app{ 800, 600, REGULAR, std::move(modelPtr) };
	//const Application app{ 800, 600, SCANLINE, std::move(modelPtr) };
	//const Application app{ 800, 600, NAIVE, std::move(modelPtr) };
	const Application app{ 800, 600, OCTREE, std::move(modelPtr) };

	app.run();
	return 0;
}