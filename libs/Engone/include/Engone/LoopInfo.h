#pragma once

namespace engone {
	class Application;
	class Window;
	// timeStep, app, window, interpolation
	struct LoopInfo {
		double timeStep=0;
		Application* app=nullptr;
		Window* window=nullptr;
		double interpolation=0;
	};
}