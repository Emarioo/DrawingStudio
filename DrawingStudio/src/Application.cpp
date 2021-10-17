#define ENGONE_DEF
#include "Engone/Rendering/Renderer.h"

#define DEFINE_HANDLER
#include "Engone/GLFWEventHandler.h"

#include "Engone/Rendering/Shader.h"

#include <iostream>

#include "Engone/UI/GraphicOverlay.h"

#include "DrawingStudio/Studio.h"
#include <chrono>
#include <thread>

int main()
{
	renderer::Init();
	input::Init(renderer::GetWindow());
	overlay::Init();
	studio::Init();
	uint32_t milli = 1000/60;
	while (!glfwWindowShouldClose(renderer::GetWindow())) {
		uint32_t now = std::chrono::system_clock::now().time_since_epoch().count()/1000;
		
		studio::Tick();

		input::ResetEvents();

		glfwPollEvents();

		uint32_t then = std::chrono::system_clock::now().time_since_epoch().count()/1000;

		if(then-now<milli)
			std::this_thread::sleep_for(std::chrono::milliseconds(milli-(then-now)));
	}
	glfwTerminate();
	return 0;
}