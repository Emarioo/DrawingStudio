#include "DrawingStudio/Studio.h"

#include <iostream>
#include <chrono>
#include <thread>

int main()
{
	using namespace engone;
	
	InitRenderer();
	InitEvents(GetWindow());
	InitGUI();
	studio::Init();
	uint32_t milli = 1000/60;
	while (!glfwWindowShouldClose(GetWindow())) {
		uint32_t now = std::chrono::system_clock::now().time_since_epoch().count()/1000;
		
		studio::Tick();

		ResetEvents();

		glfwPollEvents();

		uint32_t then = std::chrono::system_clock::now().time_since_epoch().count()/1000;

		if(then-now<milli)
			std::this_thread::sleep_for(std::chrono::milliseconds(milli-(then-now)));
	}
	glfwTerminate();
	return 0;
}