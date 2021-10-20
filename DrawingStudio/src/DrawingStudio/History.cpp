#include "History.h"

#include "Engone/EventManager.h"


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
		using namespace engone;
		AddListener(new Listener(EventType::Key, [](Event& e) {
			//std::cout << e.action << " " << e.key << "\n";
			if (e.action == 1) {
				if (IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
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