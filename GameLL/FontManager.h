#pragma once
#include "SFML/Graphics.hpp"
#include "ResourceManager.h"
class FontManager:public ResourceManager<FontManager, sf::Font>
{
public:
	FontManager():ResourceManager("fonts.cfg"){}
	sf::Font* Load(const std::string& i_path) {
		sf::Font* font = new sf::Font();
		if (!font->loadFromFile(Utils::GetWorkingDirectory() + i_path)) {
			delete font;
			font = nullptr;
		}
		return font;
	}
};

