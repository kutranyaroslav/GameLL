#pragma once
#include "S_Base.h"
#include "C_SoundEmitter.h"
#include "Directions.h"
#include "Map.h"
#include "SoundProps.h"
#include "AudioManager.h"
#include "SoundManager.h"
class S_Sound : public S_Base
{
public:
	S_Sound(SystemManager* i_systemMgr);
	~S_Sound();
	
	void Update(float i_dT);
	void HandleEvent(const EntityId& i_entity, const EntityEvent& i_event)override;
	void Notify(const Message& i_message)override;

	void SetUp(AudioManager* i_audioManager, SoundManager* i_soundManager);
private:
	sf::Vector3f MakeSoundPosition(const sf::Vector2f& i_entityPos, unsigned int i_elevation);
	void EmitSound(const EntityId& i_entity, const EntitySound& i_sound, bool i_useId, bool i_relative,
		int i_checkFrame = -1);
	AudioManager* m_audioManager;
	SoundManager* m_soundManager;
};

