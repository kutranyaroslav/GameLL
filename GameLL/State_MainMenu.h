#pragma once
#include "StateManager.h"
class State_MainMenu : public BaseState
{
public:
	State_MainMenu(StateManager* i_stateManager);
	void onCreate() override;
	void onDestroy() override;
	void Activate() override;
	void Deactivate() override;
	void Draw() override;
	void Update(const sf::Time& i_time) override;
	void MouseClick(EventDetails* i_details);
private:
	sf::Text m_text;
	sf::Vector2f m_buttonSize;
	sf::Vector2f m_buttonPos;
	unsigned int m_buttonPadding;
	sf::RectangleShape m_rects[3];
	sf::Text m_labels[3];
	sf::Font m_font;
};

