#include "application.hpp"

int main() {
	//const Application app{ 600, 600, REGULAR };
	// const Application app{ 600, 600, SCANLINE };
	//const Application app{ 600, 600, NAIVE };
	const Application app{ 600, 600, OCTREE };
	app.run();
	return 0;
}