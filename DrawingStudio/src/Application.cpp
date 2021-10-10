#define ENGONE_DEF
#include "Engone/Rendering/Renderer.h"

#define EVENT_DEF
#define EVENT_GLFW
#include "Engone/EventHandler.h"

#include "Engone/Rendering/Shader.h"

#include <iostream>

#include "Engone/UI/GraphicOverlay.h"

#include "DrawingStudio/Studio.h"

int main()
{
	renderer::Init();
	input::Init(renderer::GetWindow());
	overlay::Init();
	studio::Init();

	while (!glfwWindowShouldClose(renderer::GetWindow())) {
		input::RefreshEvents();

		studio::Tick();

		input::ResetEvents();

		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}