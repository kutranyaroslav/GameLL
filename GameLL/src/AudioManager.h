#pragma once
#include "ResourceManager.h"
#include "SFML/Audio.hpp"
#include <filesystem>
class AudioManager : public ResourceManager<AudioManager, sf::SoundBuffer>{
public: 
	AudioManager():ResourceManager("audios.cfg") {}
	sf::SoundBuffer* Load(const std::string& i_path) {
		sf::SoundBuffer* sound = new sf::SoundBuffer();
		std::string path = Utils::GetWorkingDirectory() + i_path;
		if (!sound->loadFromFile(path)) {
			delete sound;
			sound = nullptr;
		}
		
		return sound;
	}
};