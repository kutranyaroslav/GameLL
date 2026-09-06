#pragma once
#include "SFML/Graphics.hpp"
#include "ResourceManager.h"
class ShaderManager :
	public ResourceManager<ShaderManager, sf::Shader>
{
public:
	ShaderManager() :ResourceManager("shaders.cfg") {}
	sf::Shader* Load(const std::string& i_path) {
		sf::Shader* shader = new sf::Shader();
		std::string path = Utils::GetWorkingDirectory() + i_path;
		if (!shader->loadFromFile(path,sf::Shader::Fragment)) {
			delete shader;
			shader = nullptr;
			std::cerr << "! Failed to load shader: " << i_path << "\n";

		}
		return shader;
	}
};

