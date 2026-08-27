#include "S_Sound.h"


S_Sound::S_Sound(SystemManager* i_systemMgr) :
	S_Base(System::Sound, i_systemMgr), m_audioManager(nullptr), m_soundManager(nullptr) {
	Bitmask req;
	req.turnOnBit((unsigned int)Component::Position);
	req.turnOnBit((unsigned int)Component::SoundEmitter);
	m_requiredComponents.push_back(req);
	req.clearBit((unsigned int)Component::SoundEmitter);
	req.turnOnBit((unsigned int)Component::SoundEmitter);
	m_requiredComponents.push_back(req);
	m_systemMgr->GetMessageHandler()->Subscribe(EntityMessage::Direction_Changed, this);
	m_systemMgr->GetMessageHandler()->Subscribe(EntityMessage::Frame_Change, this);
	m_systemMgr->GetMessageHandler()->Subscribe(EntityMessage::State_Changed, this);

}

S_Sound::~S_Sound()
{
}

void S_Sound::SetUp(AudioManager* i_audioManager, SoundManager* i_soundManager) {
	m_audioManager = i_audioManager;
	m_soundManager = i_soundManager;
}

void S_Sound::Update(float i_dT) {
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	for (auto& entity : m_entities) {
		C_Position* pos = entities->GetComponent<C_Position>(entity, Component::Position);
		sf::Vector2f position = pos->GetPosition();
		unsigned int elevation = pos->getElevation();
		bool islistener = entities->HasComponent(entity, Component::SoundListener);
		if (islistener) {
			sf::Listener::setPosition(MakeSoundPosition(position, elevation));
		}
		if (!entities->HasComponent(entity, Component::SoundEmitter)) { return;}
		C_SoundEmitter* sound = entities->GetComponent<C_SoundEmitter>(entity, Component::SoundEmitter);
		if (sound->GetSoundId() == -1) { continue; }
		if (!islistener) {
			if (!m_soundManager->SetPosition(sound->GetSoundId(), MakeSoundPosition(position, elevation))) {
				sound->SetSoundId(-1);
			}
		}
		else {
			if (!m_soundManager->IsPlaying(sound->GetSoundId())) {
				sound->SetSoundId(-1);
			}
		}
	}
}

void S_Sound::HandleEvent(const EntityId& i_entity, const EntityEvent& i_event)
{
}

void S_Sound::Notify(const Message& i_message) {
	if (!HasEntity(i_message.m_receiver)) { return; }
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	bool isListener = entities->HasComponent(i_message.m_receiver, Component::SoundListener);
	EntityMessage m = (EntityMessage)i_message.m_type;
	switch (m)
	{
	case EntityMessage::Frame_Change: {
		if (!entities->HasComponent(i_message.m_receiver, Component::SoundEmitter)) { return; }
		EntityState state = entities->GetComponent<C_State>(i_message.m_receiver, Component::State)->GetState();
		EntitySound sound = EntitySound::None;
		switch (state)
		{
		case EntityState::Idle:
			break;
		case EntityState::Walking:
			sound = EntitySound::Footstep;
			break;
		case EntityState::Attacking:
			sound = EntitySound::Attack;
			break;
		case EntityState::Hurt:
			sound = EntitySound::Hurt;
			break;
		case EntityState::Dying:
			sound = EntitySound::Death;
			break;
		case EntityState::Changing_Axis:
			break;
		default:
			break;
		}
		EmitSound(i_message.m_receiver, sound,true, isListener, i_message.m_int);
		break;
	}
	case EntityMessage::Direction_Changed: {
		if (!isListener) { return; }
		Direction dir = (Direction)i_message.m_int;
		switch (dir)
		{
		case Direction::Right:
			sf::Listener::setDirection(1, 0, 0);
			break;
		case Direction::Left:
			sf::Listener::setDirection(-1, 0, 0);
			break;
		case Direction::Up:
			sf::Listener::setDirection(0, 0, -1);
			break;
		case Direction::Down:
			sf::Listener::setDirection(0, 0, 1);
			break;
		default:
			break;
		}
		break;
	}
	case EntityMessage::State_Changed: {
		if (i_message.m_int == i_message.m_oldState) { return; break; }
		EntitySound sound = EntitySound::None;
		switch (i_message.m_oldState) {
		case (int)EntityState::Idle:
			break;
		case (int)EntityState::Walking:
			sound = EntitySound::Footstep;
			break;
		case (int)EntityState::Attacking:
			sound = EntitySound::Attack;
			break;
		case (int)EntityState::Hurt:
			sound = EntitySound::Hurt;
			break;
		case (int)EntityState::Dying:
			sound = EntitySound::Death;
			break;
		case (int)EntityState::Changing_Axis:
			break;
		default:
			break;
		}
		StopSound(i_message.m_receiver, sound);
		break;
	}
	default:
		break;
	}
}


sf::Vector3f S_Sound::MakeSoundPosition(const sf::Vector2f& i_pos, unsigned int i_elevation) {
	return sf::Vector3f(i_pos.x, i_elevation * Sheet::Tile_Size, i_pos.y);
}

void S_Sound::EmitSound(const EntityId& i_entity, const EntitySound& i_sound, bool i_useId, bool i_relative
	, int i_checkFrame) {
	if (!HasEntity(i_entity)) { return; }
	if (!m_systemMgr->GetEntityManager()->GetComponent<C_SoundEmitter>(i_entity, Component::SoundEmitter)) { return; }
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	C_SoundEmitter* c_sound = entities->GetComponent<C_SoundEmitter>(i_entity, Component::SoundEmitter);
	if (c_sound->GetSoundId() != -1 && i_useId) { return; }
	//
	if (i_checkFrame != -1 && !c_sound->IsSoundFrame(i_sound, i_checkFrame)) { return; }
	C_Position* c_position = entities->GetComponent<C_Position>(i_entity, Component::Position);
	sf::Vector3f pos = (i_relative ? sf::Vector3f(0, 0, 0) : MakeSoundPosition(c_position->GetPosition(), c_position->getElevation()));
	if (i_useId) {
		c_sound->SetSoundId(m_soundManager->Play(c_sound->GetSound(i_sound),pos));
	}
	else {
		m_soundManager->Play(c_sound->GetSound(i_sound), pos, false, i_relative);
	}
 }

void S_Sound::StopSound(const EntityId& i_entity, const EntitySound& i_sound)
{
	if (i_sound == EntitySound::None) { return; }
	if (!HasEntity(i_entity)) { return; }
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	C_SoundEmitter* c_sound = entities->GetComponent<C_SoundEmitter>(i_entity, Component::SoundEmitter);
	if (!c_sound) { return; }

	m_soundManager->Stop(c_sound->GetSoundId());
	c_sound->SetSoundId(-1);

}
