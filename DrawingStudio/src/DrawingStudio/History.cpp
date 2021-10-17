#include "History.h"

#include "GLFW/glfw3.h"
#include "../Engone/GLFWEventHandler.h"


namespace history
{

	static std::vector<History*> histories;
	
	void AddHistory(History* history)
	{
		histories.push_back(history);
	}
	History* GetHistory()
	{
		return histories.back();
	}
	void Init()
	{
		input::AddListener(new input::Listener(input::EventType::Key, [](input::Event& e) {
			//std::cout << e.action << " " << e.key << "\n";
			if (e.action == 1) {
				if (input::IsDown(GLFW_KEY_LEFT_CONTROL)) {
					if (e.key == GLFW_KEY_Z) {
						//DrawHistory* history = GetHistory<DrawHistory>();
						if (histories.size() > 0) {
							DrawHistory* history = (DrawHistory*)histories.back();
							histories.pop_back();
							history->Revert();

							delete history;
						}
					}
					else if (e.key == GLFW_KEY_R) {

					}
				}
			}

			return false;
			}));
	}
}