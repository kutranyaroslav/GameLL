#include "../StateManager.h"
#include "../GUI_Manager.h"	
class State_Developement : public BaseState {
public:
	State_Developement(StateManager* i_stateManager);
	void onCreate()override;
	void onDestroy()override;
	void Activate()override;
	void Deactivate() override;
	void Draw()override;
	void Update(const sf::Time& i_time)override;
	void React(EventDetails* i_details);
private:
	bool RewriteConfigExcluding(const std::vector<std::string>& keysToRemove);
	bool AppendToConfig(const std::string& entry);
	unsigned int m_layerIndex;
	std::string m_currentTileset;
};