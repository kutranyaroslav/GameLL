#include "Textbox.h"
Textbox::Textbox() {
	Setup(5, 16, 200, sf::Vector2f(0, 0));
}
Textbox::Textbox(int i_visible, int i_charSize, int i_width, sf::Vector2f i_screenPos) {
	Setup(i_visible, i_charSize, i_width, i_screenPos);
}
Textbox::~Textbox() {
	Clear();
}
void Textbox::Setup(int i_visible, int i_charSize, int i_width, sf::Vector2f i_screenPos) {
	m_numVisible = i_visible;
	sf::Vector2f i_offset(2.0f, 2.0f);
	m_font.loadFromFile("D:/Programming/SFML_5/SFML_5/ARIAL.TTF");
	m_content.setFont(m_font);
	m_content.setString("");
	m_content.setCharacterSize(i_charSize);
	m_content.setFillColor(sf::Color::White);
	m_content.setPosition(i_screenPos + i_offset);

	m_backDrop.setSize(sf::Vector2f((i_visible * (i_charSize * 3.0f)), (i_visible * (i_charSize * 1.2f))));
	m_backDrop.setFillColor(sf::Color(90, 90, 90, 90));
	m_backDrop.setPosition(i_screenPos);
}

void Textbox::Add(std::string i_message) {
	m_messages.push_back(i_message);
	if (m_messages.size() < 6) { return; }
	m_messages.erase(m_messages.begin());
}
void Textbox::Clear() {
	m_messages.clear();
}
void Textbox::Render(sf::RenderWindow& i_wind) {
	std::string i_content;
	for (auto& itr : m_messages) {
		i_content.append(itr + "\n");
	}
	if (i_content != "")
	{
		m_content.setString(i_content);
		i_wind.draw(m_backDrop);
		i_wind.draw(m_content);
	}
}