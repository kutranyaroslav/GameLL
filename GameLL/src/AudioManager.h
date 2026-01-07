#pragma once
#include "ResourceManager.h"
#include "SFML/Audio.hpp"

class AudioManager : public ResourceManager<AudioManager, sf::SoundBuffer>{
public: 
	AudioManager():ResourceManager("audios.cfg") {}
	sf::SoundBuffer* Load(const std::string& i_path) {
		sf::SoundBuffer* sound = new sf::SoundBuffer();
		if (!sound->loadFromFile(Utils::GetWorkingDirectory() + i_path)) {
			delete sound;
			sound = nullptr;
		}
		return sound;
	}
};