#pragma once

/* 

GLFWEventHandler made by Emarioo/Dataolsson, Version 2.0 (2021.10.17)

What is it:
	This header features input event handling. This particular file is an event wrapper for glfw3.
	Checking input can be done by a listener or from just a function.

	Disclaimer, there may be bugs.

How to use:
	Begin by including this header into your main.cpp file and add #define DEFINE_HANDLER.
	#include glfw3.h before this header!

	The Init() should be called before other functions in this header. Preferable before the game loop.
	Init(GlfWwindow) needs a window from glfw to set the callbacks.

	Use ResetInput() at the end of your game loop to reset certain events.

Other:
	You can use and edit this however you want but give credit where credit is due.

Versions:
	1.0: First edition, listeners, functions to detect mouse, key scroll states. Console and GLFW is merged here.
	2.0: Seperated Console and GLFW into seperate files.
*/

#include <functional>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>

namespace input
{
	/*==============
	//  Features  //
	==============*/
	

	/*
	Init usage of glfw3.
	*/
	void Init(GLFWwindow* window);

	typedef char EventTypes;

	enum class EventType : char
	{
		NONE = 0,
		Click = 1,
		Move = 2,
		Scroll = 4,
		Key = 8,
		Resize = 16,
		Focus = 32,
	};

	inline EventType operator|(EventType a, EventType b);
	inline bool operator==(EventType a, EventType b);

	//extern EventTypes ClickEvent,MoveEvent,ScrollEvent,KeyEvent,ResizeEvent,FocusEvent;

	struct Keybinding
	{
		int keys[3];
	};
	class Event
	{
	public:
		Event() = default;
		Event(EventType types);

		EventType eventType;

		union
		{
			struct // key
			{
				int action, key, scancode;
			};
			struct // mouse click move
			{
				int action, button, mx, my;
			};
			struct // mouse scroll
			{
				float scrollX, scrollY;
				int mx, my;
			};
			struct // Event functions
			{
				// from the resize event
				int width, height;
				bool focused;
			};
		};
	};
	/*
	Cannot listen for multiple different events that uses the same member variables in the union.
	Mouse click, move and scroll events are fine but not mouse and key events.
	Use EventType::Click|EventType::Move|EventType::Scroll as first argument to listen to multiple events
	The bool of the function will stop an event from being listened from other listeners.
	*/
	class Listener
	{
	public:
		Listener(EventType eventTypes, std::function<bool(Event&)> f);
		/*
		@priority determines if this listener should be called first when handling events. A high number is more important.
		This takes effect if the listener's function returns true which will stop other listeners.
		*/
		Listener(EventType eventTypes, int priority, std::function<bool(Event&)> f);
		std::function<bool(Event&)> run;
		int priority = 0;
		EventType eventTypes;
	};

	void AddListener(Listener* listener);

	// Use this at the end of you gameloop.
	void ResetEvents();

	int GetMouseX();
	int GetMouseY();
	
	//-- Dynamic event/input checking
	/*
	virtualKeyCode: Windows.h or glfw virtual key code depending on what you are using.
	return true if the key or mouse button is down/held.
	*/
	bool IsDown(int virtualKeyCode);
	/*
	virtualKeyCode: Windows.h or glfw virtual key code depending on what you are using.
	return true if the key was pressed this refresh/frame/update.
	*/
	bool IsPressed(int virtualKeyCode);
	/*
	return 0 if no scroll and 1 or -1 if scrolled.
	*/
	int IsScrolledY();
	int IsScrolledX();

	//-- Keybindings
	bool IsKeybindingDown(short id);
	void AddKeybinding(short id, int keyCode0, int keyCode1, int keyCode2);
	/*
	Returns number of keys loaded. Zero means that an empty keybindings file was loaded or that there is no file. Either way, default keybindings should be created.
	Less keybindings than there should be will most likely cause issues in the gameplay.
	@path is the path of the file. data/keybindings.dat for example. The format is irrelevant since it will use binary either way.
	*/
	int LoadKeybindings(const std::string& path);
	void SaveKeybindings(const std::string& path);
	void ClearKeybindings();



	/*====================
	//  Implementation  //
	====================*/

#ifdef DEFINE_HANDLER
	struct Input
	{
		int code;
		bool down = false;
		int pressed = false;
	};
	static int mouseX, mouseY;
	static float scrollX, scrollY;
	static std::unordered_map<short, Keybinding> keybindings;
	static std::vector<Listener*> listeners;
	static std::vector<Event> events;
	static std::vector<Input> inputs;

	inline EventType operator|(EventType a, EventType b)
	{
		return (EventType)((char)a | (char)b);
	}
	inline bool operator==(EventType a, EventType b)
	{
		//std::cout << int(a) << " - " << int(b)<<" "<<(((char)a & (char)b)>0) << "\n";
		return (char(a) & char(b))>0;
	}

	Event::Event(EventType type) : eventType(type) {}
	Listener::Listener(EventType eventTypes, std::function<bool(Event&)> f)
		: eventTypes(eventTypes), run(f){}
	Listener::Listener(EventType eventTypes,int priority, std::function<bool(Event&)> f)
		: eventTypes(eventTypes), run(f), priority(priority){ }
	void SetInput(int code, bool down)
	{
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code) {
				if (down) {
					if (!inputs[i].down) {
						inputs[i].down = down;
						inputs[i].pressed++;
						return;
	}
				}
				else {
					inputs[i].down = false;
				}
				break;
			}
		}
		if (down)
			inputs.push_back({ code, down, 1 });
	}
	void ExecuteListeners()
	{
		char breaker=0;
		for (int j = 0; j < events.size(); j++) {
			for (int i = 0; i < listeners.size(); i++) {
				if((char)listeners[i]->eventTypes&breaker)
					continue;

				if ((char)listeners[i]->eventTypes & (char)events[j].eventType) {
					//std::cout << "hm\n";
					if(listeners[i]->run(events[j]))
						breaker = (char)(breaker | (char)listeners[i]->eventTypes);
				}
			}
		}
		while (events.size() > 0)
			events.pop_back();
	}

	void Init(GLFWwindow* window)
	{
		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			Event e(EventType::Key);
			e.key = key;
			e.scancode = scancode;
			e.action = action;
			events.push_back(e);
			SetInput(key, action == 1);
			ExecuteListeners();
			});
		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			Event e(EventType::Click);
			e.button = button;
			e.action = action;
			e.mx = mouseX;
			e.my = mouseY;
			events.push_back(e);
			SetInput(button, action==1);
			ExecuteListeners();
			});
		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double mx, double my) {
			mouseX = mx;
			mouseY = my;
			Event e(EventType::Move);
			e.mx = mx;
			e.my = my;
			events.push_back(e);
			ExecuteListeners();
			});
		glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
			Event e(EventType::Scroll);
			e.scrollX = xoffset;
			e.scrollY = yoffset;
			e.mx = mouseX;
			e.my = mouseY;
			events.push_back(e);
			//std::cout << xoffset << " " << yoffset << "\n";
			scrollX = xoffset;
			scrollY = yoffset;
			ExecuteListeners();
			});
		glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focus) {
			Event e(EventType::Focus);
			e.focused = focus;
			events.push_back(e);
			ExecuteListeners();
			});
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
			Event e(EventType::Resize);
			e.width = width;
			e.height = height;
			events.push_back(e);
			ExecuteListeners();
			});
	}
	void AddListener(Listener* listener)
	{
		// Prevent duplicates
		for (int i = 0; i < listeners.size(); i++) {
			if (listener == listeners[i]) {
				return;
			}
		}
		// Priorities
		for (int i = 0; i < listeners.size(); i++) {
			if (listener->priority > listeners[i]->priority) {
				listeners.insert(listeners.begin() + i, listener);
				return;
			}
		}
		listeners.push_back(listener);
	}
	int GetMouseX(){return mouseX;}
	int GetMouseY(){return mouseY;}
	bool IsDown(int code)
	{
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code)
				return inputs[i].down;
		}
		return false;
	}
	bool IsPressed(int code)
	{
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code)
				return inputs[i].pressed > 0;
		}
		return false;
	}
	int IsScrolledY()
	{
		if (scrollY != 0)
			return scrollY;
		return 0;
	}
	int IsScrolledX()
	{
		if (scrollX != 0)
			return scrollX;
		return 0;
	}
	void ResetEvents()
	{
		scrollX = 0;
		scrollY = 0;
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].pressed > 0)
				inputs[i].pressed--;
		}
	}
	bool IsKeybindingDown(short id)
	{
		if (keybindings.count(id)) {
			Keybinding& bind = keybindings[id];
			if (IsDown(bind.keys[0])) {
				if (bind.keys[1] == 0) {
					return true;
				} else if(IsDown(bind.keys[1])) {
					if (bind.keys[2]==0) {
						return true;
					}
					else if(IsDown(bind.keys[2])) {
						return true;
					}
				}
			}
		}
		return false;
	}
	void AddKeybinding(short id, int key0=0, int key1=0, int key2=0)
	{
		keybindings[id] = {key0,key1,key2};
	}
	int LoadKeybindings(const std::string& path)
	{
		std::ifstream file(path, std::ios::binary);

		if (!file) {
			file.close();
			return 0;
		}

		file.seekg(0, file.end);
		int size = file.tellg();
		file.seekg(0, file.beg);

		struct keyset
		{
			short id;
			int keys[3];
		};

		int numKeys = (int)(size / (sizeof(keyset)));

		keyset* sets = new keyset[numKeys];

		file.read(reinterpret_cast<char*>(&sets[0]), sizeof(keyset) * numKeys);

		for (int i = 0; i < numKeys; i++) {
			AddKeybinding(sets[i].id, sets[i].keys[0],sets[i].keys[1],sets[i].keys[2]);
		}

		// Cleanup
		delete[] sets;
		file.close();

		return numKeys;
	}
	void SaveKeybindings(const std::string& path)
	{
		std::ofstream file(path, std::ios::binary);

		int numKeys = keybindings.size();

		struct keyset
		{
			short id;
			int keys[3];
		};

		keyset* sets = new keyset[numKeys];

		int i = 0;
		for (auto& pair : keybindings) {
			sets->id = pair.first;
			memcpy_s(sets->keys,3,pair.second.keys,3);
			i++;
		}

		file.write(reinterpret_cast<const char*>(&sets[0]), sizeof(keyset) * numKeys);

		// Cleanup
		delete[] sets;
		file.close();
	}
	void ClearKeybindings(){keybindings.clear();}
	static bool lastL = false, lastM = false, lastR = false;
#endif
}