#pragma once
#include "StateManager.h"
#include "dev/ErrorLogManager.h"
class State_Paused:public BaseState
{
public:
	State_Paused(StateManager* i_stateManager);
	void onCreate() override;
	void onDestroy() override;
	void Activate() override;
	void Deactivate() override;
	void Update(const sf::Time& i_time) override;
	void Draw() override;
	void Unpause(EventDetails* i_details);
private:
	sf::Text m_text;
	sf::RectangleShape m_rect;
};

