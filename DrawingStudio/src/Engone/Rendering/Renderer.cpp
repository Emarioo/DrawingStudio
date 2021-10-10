#include "renderer.h"

static GLFWwindow* window;
int Width()
{
	int w;
	glfwGetWindowSize(window,&w,nullptr);
	return w;
}
int Height()
{
	int h;
	glfwGetWindowSize(window, nullptr, &h);
	return h;
}
GLFWwindow* Init()
{
	if (!glfwInit()) {
		return nullptr;
	}
	window = glfwCreateWindow(500, 500, "Drawing Studio", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		return nullptr;
	}
	return window;
}