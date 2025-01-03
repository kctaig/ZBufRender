#include "application.hpp"

int main() {
	auto start = std::chrono::high_resolution_clock::now();
	/********************** choose model to render ************************/

	//auto modelPtr = std::make_unique<Model>(R"(../../asserts)", "suzanne_1k.obj");
	//auto modelPtr = std::make_unique<Model>(R"(../../asserts)", "teapot_6k.obj");
	//auto modelPtr = std::make_unique<Model>(R"(../../asserts)", "bunny_69k.obj");
	auto modelPtr = std::make_unique<Model>(R"(../../asserts)", "armadillo_212k.obj");

	/****************************** end *********************************/
	auto end = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "model loading time : " << duration << " ms" << std::endl;

	/*********************** choose render type **************************/

	//const Application app{ 800, 600, REGULAR, std::move(modelPtr) };
	//const Application app{ 800, 600, SCANLINE, std::move(modelPtr) };
	const Application app{ 800, 600, NAIVE, std::move(modelPtr) };
	//const Application app{ 800, 600, OCTREE, std::move(modelPtr) };

	/****************************** end *******************************/

	app.run();
	return 0;
}