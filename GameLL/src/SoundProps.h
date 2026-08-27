#pragma once
#include <SFML/Audio.hpp>
#include <array>
#include "StateManager.h"
struct SoundProps {
	std::vector<std::string> m_audioNames;   
	const std::string& GetRandomAudioName() const {
		return m_audioNames[Utils::RandomIndex(m_audioNames.size())];
	}
	float GetRandomPitch() const {
		return m_pitchVariance <= 0.f ? m_pitch
			: Utils::RandomRange(m_pitch - m_pitchVariance, m_pitch + m_pitchVariance);
	}
	float m_volume = 100.f;
	float m_pitch = 1.f;          
	float m_pitchVariance = 0.f;  
	float m_attenuation = 1.f;
	float m_minDistance = 1.f;
	
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