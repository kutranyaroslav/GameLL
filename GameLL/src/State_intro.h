#pragma once
#include "StateManager.h"
class State_intro:public BaseState
{
public:
	State_intro(StateManager* i_stateManager);
	void Continue(EventDetails* i_details);
	void onCreate() override;
	void onDestroy() override;
	void Activate() override;
	void Deactivate() override;
	void Update(const sf::Time& i_time) override;
	void Draw() override;
private:
	sf::Sprite m_introSprite;
	sf::Text m_text;
	sf::Font m_font;
	float m_timePassed;
};

