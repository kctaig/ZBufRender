#include "application.hpp"

int main() {
	//const Application app{ 800, 600, REGULAR };
	const Application app{ 800, 600, SCANLINE };
	app.run();
	return 0;
}