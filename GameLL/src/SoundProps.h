#pragma once
#include <SFML/Audio.hpp>
#include <array>
#include "StateManager.h"
struct SoundProps {
	SoundProps(const std::string& i_name = "") :m_audioName(i_name), m_volume(100), m_pitch(1.f),
	m_minDistance(10.f), m_attenuation(10.f){}
	std::string m_audioName;
	float m_volume;
	float m_pitch;
	float m_minDistance;
	float m_attenuation;
};



struct SoundInfo {
	SoundInfo(const std::string& i_name): m_name(i_name), m_manualPaused(false){}
	std::string m_name;
	bool m_manualPaused;
};


using SoundId = int; 
using SoundProperties = std::unordered_map<std::string, SoundProps>;
using SoundContainer = std::unordered_map<SoundId, std::pair<SoundInfo, sf::Sound*>>;
using Sounds = std::unordered_map<StateType, SoundContainer>;
using RecycledSound = std::vector<std::pair<std::pair<SoundId, std::string>, sf::Sound*>>;
using MusicContainer = std::unordered_map<StateType, std::pair<SoundInfo, sf::Music*>>;

enum class EntitySound { None = -1, Footstep,Attack,Hurt,Death };

struct SoundParameters {
	static const int Max_SoundFrames = 5;
	SoundParameters() {
		for (int i = 0; i < Max_SoundFrames; ++i) { m_frames[i] = -1; }
	}
	std::string m_sound;
	std::array<int, Max_SoundFrames> m_frames;
};