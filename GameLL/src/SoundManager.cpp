#include "SoundManager.h"

SoundManager::SoundManager(AudioManager* i_audioMgr) :m_audioMgr(i_audioMgr),m_lastId(0)
, m_elapsed(0.f), m_numSounds(0), m_currentState(StateType(0))
{}
SoundManager::~SoundManager() { CleanUp(); }

void SoundManager::ChangeState(const StateType& i_state) {
	PauseAll(m_currentState);
	UnpauseAll(i_state);
	m_currentState = i_state;
	if (m_music.find(i_state) != m_music.end()) { return; }
	SoundInfo info("");
	sf::Music* music = nullptr;
	m_music.emplace(i_state, std::make_pair(info,music));
}
void SoundManager::RemoveState(const StateType& i_state) {
	auto& stateSounds = m_audio.find(i_state)->second;
	for (auto& itr : stateSounds) {
		RecycleSound(itr.first, itr.second.second, itr.second.first.m_name);
	}
	m_audio.erase(i_state);
	auto stateMusic = m_music.find(i_state);
	if (stateMusic == m_music.end()) { return; }
	if (stateMusic->second.second) {
		delete stateMusic->second.second;
		stateMusic->second.second = nullptr;
		--m_numSounds;
	}
	m_music.erase(stateMusic);
}
void SoundManager::CleanUp() {
	for (auto& state : m_audio) {
		for (auto& sound : state.second) {
			m_audioMgr->ReleaseResource(sound.second.first.m_name);
			delete sound.second.second;
			sound.second.second = nullptr;
		}
	}
	m_audio.clear();
	for (auto& recycled : m_recycled) {
		m_audioMgr->ReleaseResource(recycled.first.second);
		delete recycled.second;
		recycled.second = nullptr;
	}
	m_recycled.clear();
	for (auto& state : m_music) {
		if (state.second.second) {
			delete state.second.second;
			state.second.second = nullptr;
		}
	}
	m_music.clear();
	m_properties.clear();
	m_numSounds = 0;
	m_lastId = 0;
}

void SoundManager::Update(float i_dT) {
	m_elapsed += i_dT;
	//one run in 3 secs 
	if (m_elapsed < 0.33f) { return; }
	m_elapsed = 0;
	auto& container = m_audio.at(m_currentState);
	for (auto itr = container.begin(); itr != container.end();) {
		if (!itr->second.second->getStatus()) {
			RecycleSound(itr->first, itr->second.second, itr->second.first.m_name);
			itr = container.erase(itr);
			continue;
		}
		itr++;
	}
	auto music = m_music.find(m_currentState);
	if (music == m_music.end()) { return; }
	if (!music->second.second) { return; }
	if (music->second.second->getStatus()) { return; }
	delete music->second.second;
	music->second.second = nullptr;
	--m_numSounds;
}



SoundId SoundManager::Play(const std::string& i_sound, const sf::Vector3f& i_pos,
	bool i_loop, bool i_relative, const std::string& i_overrideSound) {
	
	SoundProps* props = GetSoundProperties(i_sound);
	if (!props) { return -1; }
	SoundId id;
	sf::Sound* sound = nullptr;
	for (auto& itr : m_audio.at(m_currentState)) {
		for (auto& audio : props->m_audioNames) {
			if (itr.second.first.m_name == audio) {
				id = itr.first;
				sound = itr.second.second;
			}
		}
	}
	const std::string& audio = props->GetRandomAudioName();
	if (!sound) { sound = CreateSound(id, audio); }
	if (!sound) { return -1; }
	SetUpSound(sound, props, i_loop, i_relative);
	sound->setPosition(i_pos);
	SoundInfo info(audio);
	m_audio.at(m_currentState).emplace(id,std::make_pair(info, sound));
	sound->play();
	return id;
}

bool SoundManager::Play(const SoundId& i_sound) {
	auto container = m_audio.find(m_currentState);
	if (container == m_audio.end()) { return false; }
	auto sound = container->second.find(i_sound);
	if (sound == container->second.end()) { return false; }
	sound->second.second->play();
	sound->second.first.m_manualPaused = false;
	return true;

}
bool SoundManager::Pause(const SoundId& i_sound) {
	auto container = m_audio.find(m_currentState);
	if (container == m_audio.end()) { return false; }
	auto sound = container->second.find(i_sound);
	if (sound == container->second.end()) { return false; }
	sound->second.second->pause();
	sound->second.first.m_manualPaused = true;
	return true;
}
bool SoundManager::Stop(const SoundId& i_sound) {
	if (i_sound == -1) { return false; }
	auto container = m_audio.find(m_currentState);
	if (container == m_audio.end()) { return false; }
	auto sound = container->second.find(i_sound);
	if (sound == container->second.end()) { return false; }
	sound->second.second->stop();
	sound->second.first.m_manualPaused = true;
	return true;
}

bool SoundManager::PlayMusic(const std::string& i_musicId, float i_volume, bool i_loop) {
	auto s = m_music.find(m_currentState);
	if (s == m_music.end()) { return false; }
	std::string path = m_audioMgr->GetPath(i_musicId);
	if (path == "") { return false; }
	if (!s->second.second) {
		s->second.second = new sf::Music();
		++m_numSounds;
	}
	sf::Music* music = s->second.second;
	if (!music->openFromFile(Utils::GetWorkingDirectory() + path)) {
		delete music;
		music = nullptr;
		--m_numSounds;
		return false;
	}
	music->setLoop(i_loop);
	music->setVolume(i_volume);
	music->setRelativeToListener(true);
	music->play();
	s->second.first.m_name = i_musicId;
	return true;
}
bool SoundManager::PlayMusic(const StateType& i_state) {
	auto music = m_music.find(i_state);
	if (music == m_music.end()) { return false; }
	if (!music->second.second) { return false; }
	music->second.second->play();
	music->second.first.m_manualPaused = false;
	return true; 
} 
bool SoundManager::StopMusic(const StateType& i_state) {
	auto music = m_music.find(i_state);
	if (music == m_music.end()) { return false; }
	if (!music->second.second) { return false; }
	music->second.second->stop();
	delete  music->second.second;
	music->second.second = nullptr;
	--m_numSounds;
	return true;

}
bool SoundManager::PauseMusic(const StateType& i_state) {
	auto music = m_music.find(i_state);
	if (music == m_music.end()) { return false; }
	if (!music->second.second) { return false; }
	music->second.second->pause();
	music->second.first.m_manualPaused = true;
	return true;
}

bool SoundManager::SetPosition(const SoundId& i_sound, const sf::Vector3f& i_pos) {
	auto& container = m_audio[m_currentState];
	auto sound = container.find(i_sound);
	if (sound == container.end()) { return false; }
	sound->second.second->setPosition(i_pos);
	return true;
}

bool SoundManager::IsPlaying(const SoundId& i_sound) {
	auto& container = m_audio[m_currentState];
	auto sound = container.find(i_sound);
	return sound == container.end() ? false : sound->second.second->getStatus();
}

SoundProps* SoundManager::GetSoundProperties(const std::string& i_soundName) {
	auto soundprops = m_properties.find(i_soundName);
	if (soundprops == m_properties.end()) {
		if (!LoadProperties(i_soundName)) { return nullptr; }
		soundprops = m_properties.find(i_soundName);
	}
	return &soundprops->second;
}

const SoundId& SoundManager::GetSoundId(const std::string& i_soundName)
{
	for(auto& itr: m_audio[m_currentState]){
		if (itr.second.first.m_name == i_soundName) { return itr.first; }
	}
	return -1;
}

bool SoundManager::LoadProperties(const std::string& i_name) {
	std::ifstream file; 
	file.open(Utils::GetWorkingDirectory() +"Assets//SoundsFiles//" + i_name + ".sound");
	if (!file.is_open()) { return false; }
	SoundProps props;
	std::string line;
	while (std::getline(file, line)) {
		if (line[0] == '|') { continue; }
		std::stringstream keystream(line);
		std::string key; 
		keystream >> key;
		if (key == "Audio") {
			std::string name;
			keystream >> name;
			props.m_audioNames.push_back(name);
		}
		else if (key == "Volume") {
			keystream >> props.m_volume;
		}
		else if (key == "Pitch") {
			keystream >> props.m_pitch;
		}
		else if (key == "Attenuation") {
			keystream >> props.m_attenuation;
		}
		else if (key == "Distance") {
			keystream >> props.m_minDistance;
		}
		
	}
	file.close();
	if (props.m_audioNames.empty()) { return false; }
	m_properties.emplace(i_name, props);
	return true;
}

void SoundManager::PauseAll(const StateType& i_state) {
	auto& container = m_audio[i_state];
	for (auto itr = container.begin(); itr != container.end();) {
		if (!itr->second.second->getStatus()) {
			RecycleSound(itr->first, itr->second.second, itr->second.first.m_name);
			itr = container.erase(itr);
			continue;
		}
		itr->second.second->pause();
		++itr;
	}
	auto music = m_music.find(i_state);
	if (music == m_music.end()) { return; }
	if (!music->second.second) { return; }
	music->second.second->pause();

}

void SoundManager::UnpauseAll(const StateType& i_state) {
	auto& container = m_audio[i_state];
	for (auto& itr: container){
		if (itr.second.first.m_manualPaused) { continue; }
		itr.second.second->play();
	}
	auto music = m_music.find(i_state);
	if (music == m_music.end()) { return; }
	if (!music->second.second) { return; }
	if (music->second.first.m_manualPaused) { return; }
	music->second.second->play();

}

sf::Sound* SoundManager::CreateSound(SoundId& i_id, const std::string& i_audioName) {
	sf::Sound* sound = nullptr;
	if (!m_recycled.empty() && (m_numSounds >= Max_Sounds || m_recycled.size() >= Sound_Cache)) {
		auto itr = m_recycled.begin();
		while (itr != m_recycled.end()) {
			if (itr->first.second == i_audioName) { break; }
			++itr;
		}
		if (itr == m_recycled.end()) {
			auto element = m_recycled.begin();
			i_id = element->first.first;
			m_audioMgr->ReleaseResource(element->first.second);
			m_audioMgr->RequireResource(i_audioName);
			sound = element->second;
			sound->setBuffer(*m_audioMgr->GetResource(i_audioName));
			m_recycled.erase(element);
		}
		else {
			i_id = itr->first.first;
			sound = itr->second;
			m_recycled.erase(itr);
		}
		return sound;
	}
	if (m_numSounds < Max_Sounds) {
		if (m_audioMgr->RequireResource(i_audioName)) {
			sound = new sf::Sound;
			i_id = m_lastId;
			++m_lastId;
			++m_numSounds;
			sound->setBuffer(*m_audioMgr->GetResource(i_audioName));
			return sound;
		}
	}
	return nullptr;
}

void SoundManager::SetUpSound(sf::Sound* i_sound, const SoundProps* i_props, bool i_loop, bool i_relative) {
	i_sound->setVolume(i_props->m_volume);
	i_sound->setAttenuation(i_props->m_attenuation);
	i_sound->setMinDistance(i_props->m_minDistance);
	i_sound->setPitch(i_props->GetRandomPitch()); 
	i_sound->setLoop(i_loop);
	i_sound->setRelativeToListener(i_relative);
}

bool SoundManager::RecycleSound(const SoundId& i_id, sf::Sound* i_sound, const std::string& i_soundName) {
	 return m_recycled.emplace_back(std::make_pair(std::make_pair(i_id, i_soundName), i_sound)).second;
}