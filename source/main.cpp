#include "application.hpp"

int main() {
	auto start = std::chrono::high_resolution_clock::now();
	/********************** choose model to render ************************/

	//auto modelPtr = std::make_unique<Model>(R"(../../asserts)", "suzanne_1k.obj");
	//auto modelPtr = std::make_unique<Model>(R"(../../asserts)", "spot_5k.obj");
	//auto modelPtr = std::make_unique<Model>(R"(../../asserts)", "bunny_69k.obj");
	//auto modelPtr = std::make_unique<Model>(R"(../../asserts)", "armadillo_212k.obj");
	auto modelPtr = std::make_unique<Model>(R"(../../asserts)", "dragon_871k.obj");

	/****************************** end *********************************/
	auto end = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "model loading time : " << duration << " ms" << std::endl;

	/*********************** choose render type **************************/

	//const Application app{ 800, 600, REGULAR, std::move(modelPtr) };
	//const Application app{ 800, 600, SCANLINE, std::move(modelPtr) };
	//const Application app{ 800, 600, NAIVEHIZ, std::move(modelPtr) };
	const Application app{ 800, 600, OCTREEHIZ, std::move(modelPtr) };
	//const Application app{ 800, 600, KDTREEHIZ, std::move(modelPtr) };

	/****************************** end *******************************/

	app.run();
	return 0;
}