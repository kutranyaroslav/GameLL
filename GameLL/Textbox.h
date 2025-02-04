#pragma once
#include <vector>
#include <string>
#include "SFML/Graphics.hpp"

using messageContainer = std::vector<std::string>;

class Textbox
{
public:
	Textbox();
	Textbox(int i_visible, int i_charSize, int i_width, sf::Vector2f i_screenPos);
	~Textbox();
	void Setup(int i_visible, int i_charSize, int i_width, sf::Vector2f i_screenPos);
	void Add(std::string i_message);
	void Clear();
	void Render(sf::RenderWindow& i_wind);
private:
	messageContainer m_messages;
	int m_numVisible;
	sf::RectangleShape m_backDrop;
	sf::Font m_font;
	sf::Text m_content;

};

