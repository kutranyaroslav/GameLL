#pragma once
#include "SoundProps.h"
#include "AudioManager.h"

class SoundManager
{
public:
	SoundManager(AudioManager* i_audioMgr);
	~SoundManager();

	void ChangeState(const StateType& i_state);
	void RemoveState(const StateType& i_state);
	void Update(float i_dT);
	SoundId Play(const std::string& i_sound, const sf::Vector3f& i_pos,
		bool i_loop = false, bool i_relative = false);
	bool Play(const SoundId& i_sound);
	bool Pause(const SoundId& i_sound);
	bool Stop(const SoundId& i_sound);

	bool PlayMusic(const std::string& i_musicId, float i_volume = 100.f, bool i_loop = false);
	bool PlayMusic(const StateType& i_state);
	bool StopMusic(const StateType& i_state);
	bool PauseMusic(const StateType& i_state);

	bool SetPosition(const SoundId& i_sound, const sf::Vector3f& i_pos);
	bool IsPlaying(const SoundId& i_sound);
	SoundProps* GetSoundProperties(const std::string& i_soundName);

	//getters
	const SoundId& GetSoundId(const std::string& i_soundName);

	static const int Max_Sounds = 150;
	static const int Sound_Cache = 75;
private:
	bool LoadProperties(const std::string& i_name);
	void PauseAll(const StateType& i_state);
	void UnpauseAll(const StateType& i_state);

	sf::Sound* CreateSound(SoundId& i_id, const std::string& i_audioName);
	void SetUpSound(sf::Sound* i_sound, const SoundProps* i_props,
		bool i_loop = false, bool i_relative = false);
	bool RecycleSound(const SoundId& i_id, sf::Sound* i_sound, const std::string& i_soundName);
	void CleanUp();
	
	Sounds m_audio;
	MusicContainer m_music;
	RecycledSound m_recycled;
	SoundProperties m_properties;
	StateType m_currentState;
	SoundId m_lastId;
	unsigned int m_numSounds;
	float m_elapsed;
	AudioManager* m_audioMgr;
};

