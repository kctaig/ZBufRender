#include "application.hpp"

int main() {
	//const Application app{ 800, 600, REGULAR };
	//const Application app{ 800, 600, SCANLINE };
	const Application app{ 800, 600, NAIVE };
	app.run();
	return 0;
}