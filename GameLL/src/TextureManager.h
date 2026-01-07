#pragma once
#include "SFML/Graphics.hpp"
#include "ResourceManager.h"
class TextureManager:public ResourceManager<TextureManager, sf::Texture>
{
public:
	TextureManager():ResourceManager("textures.cfg"){}
	sf::Texture* Load(const std::string& i_path) {
		sf::Texture* texture = new sf::Texture();
		if (!texture->loadFromFile(Utils::GetWorkingDirectory() + i_path)) {
			delete texture;
			texture = nullptr;
			std::cerr << "! Failed to load texture: " << i_path << "\n";

		}
		return texture;
	}
};

