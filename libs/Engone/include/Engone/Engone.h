#pragma once

// #include "Engone/RenderModule.h"
#include "Engone/Application.h"
#include "Engone/Window.h"

#include "Engone/PlatformModule/GameMemory.h"

// #include "Engone/SoundModule.h"
// #include "Engone/Utilities/Utilities.h"
// #include "Engone/Utilities/MemAllocator.h"
// #include "Engone/Networking/NetworkModule.h"

#include "Engone/Rendering/FrameBuffer.h"
#include "Engone/Rendering/Light.h"

// #include "Engone/Rendering/Camera.h"

// #include "Engone/Utilities/rp3d.h"
// #include "Engone/EngineObject.h"
// #include "Engone/Utilities/RuntimeStats.h"
// #include "Engone/ParticleModule.h"

namespace engone {
	class Engone {
	public:
		Engone();
		~Engone();

		enum EngoneFlag : uint32_t {
			EngoneNone = 0,
			EngoneShowHitboxes=1,
			EngoneEnableDebugInfo = 2, // not used
			EngoneShowBasicDebugInfo = 4, // not used
			EngoneRenderOnResize = 8,
		};
		typedef uint32_t EngoneFlags;

		// Will create an application in the engine.
		// The created app will be tracked which means it requires TrackerId. I would recommend looking at an Application example.
		void addApplication(Application* app);

		// I wouldn't recommend adding or removing apps from the vector.
		// Instead use createApplication or Application::stop if you want to add or remove an app.
		std::vector<Application*>& getApplications();

		/*
		Start the engine loop which will run applications until their windows are closed.
		*/
		void start();

		void manageThreading();
		void manageNonThreading();

		void renderApps();

		// the run time of stats
		//inline double getEngineTime() const { return m_runtimeStats.getRunTime(); }

		void addLight(Light* l);
		void removeLight(Light* l);

		inline EngoneFlags getFlags() const {
			return m_flags;
		}
		inline void setFlags(EngoneFlags flags) {
			m_flags = flags;
		}

		//RuntimeStats& getStats() { return m_runtimeStats; }
		
		void bindLights(Shader* shader, glm::vec3 objectPos);
		// returns current loop info
		LoopInfo& getLoopInfo();

		inline ExecutionTimer& getUpdateTimer() { return mainUpdateTimer; };
		inline ExecutionTimer& getRenderTimer() { return mainRenderTimer; };
		
		void saveGameMemory(const std::string& path);
		void loadGameMemory(const std::string& path);

		// static TrackerId trackerId;
	private:
		//RuntimeStats m_runtimeStats;

		EngoneFlags m_flags;

		// delta uniform is set in engine loop.
		std::vector<Light*> m_lights;

		bool m_loadedDefault=false;

		FrameBuffer frameBuffer;
		VertexBuffer quadBuffer;
		VertexArray quadArray;

		// Apps use these when not multithreaded
		ExecutionTimer mainUpdateTimer;
		ExecutionTimer mainRenderTimer;

		LoopInfo currentLoopInfo;

		//GameMemory gameMemory;

		std::vector<Application*> m_applications{};

		// Todo: instead of 3 vectors, use one vector<Tracker::TrackClass>
		std::vector<uint16_t> m_appSizes{}; // used for tracker
		// std::vector<TrackerId> m_appIds{}; // used for tracker

		void update(LoopInfo& info);
		// only renders objects at the moment
		void render(LoopInfo& info);
		void renderObjects(LoopInfo& info);

		friend class Application;
		friend class Window;
	};
}