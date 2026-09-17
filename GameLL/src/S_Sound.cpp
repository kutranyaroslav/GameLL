#include "S_Sound.h"
#include "World.h"

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
	
	//initialize the materials by default
	LoadMaterials();
}

S_Sound::~S_Sound()
{
}

void S_Sound::SetUp(AudioManager* i_audioManager, SoundManager* i_soundManager) {
	m_audioManager = i_audioManager;
	m_soundManager = i_soundManager;
}

void S_Sound::LoadMaterial(const std::string& i_materialName)
{
	std::string path = Utils::GetWorkingDirectory() + "//src//Materials//" + i_materialName + ".material";
	std::ifstream file(path);
	if (!file.is_open()) { return; }
	std::string line;
	Materials::Material material(i_materialName);
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string key;
		std::string audio;
		ss >> key;
		if (key == "Sound") {
			ss >> audio;
			material.m_soundNames.push_back(audio);
		}
	}
	m_materials.emplace(i_materialName, material);
}

void S_Sound::LoadMaterials()
{
	std::string path = Utils::GetWorkingDirectory() + "src//Materials/";
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (entry.path().extension() == ".material") {
			LoadMaterial(entry.path().stem().string());
		}
	}
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

		//the material of the tile under the entity overrides its footstep sound
		std::string materialSound = "";
		if (sound == EntitySound::Footstep)
		{
			C_Position* pos = entities->GetComponent<C_Position>(i_message.m_receiver, Component::Position);
			Tile* tile = GetTileUnder(pos);
			if (tile) {
				for (auto& itr : tile->m_properties->m_materialTags) {
					materialSound = GetMaterialSound(itr);
					if (materialSound != "") { break; }
				}
			}
		}
			//one emit: the material sound when there is one, the emitter's own otherwise
			EmitSound(i_message.m_receiver, sound, true, isListener, i_message.m_int, materialSound);
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



//topmost tile under the entity: slides sit below it, so the first hit going down wins
Tile* S_Sound::GetTileUnder(C_Position* i_pos) {
	if (!i_pos) { return nullptr; }
	SharedContext* context = m_systemMgr->GetSharedContext();
	if (!context || !context->m_world) { return nullptr; }
	Map* map = context->m_world->GetCurrentMap();
	if (!map) { return nullptr; }
	unsigned int x = static_cast<unsigned int>(i_pos->GetPosition().x / Sheet::Tile_Size);
	unsigned int y = static_cast<unsigned int>(i_pos->GetPosition().y / Sheet::Tile_Size);
	//cast before the subtraction: getElevation() is unsigned and 0 - 1 wraps around
	for (int layer = static_cast<int>(i_pos->getElevation()) - 1; layer >= 0; --layer) {
		Tile* tile = map->GetTile(x, y, layer);
		if (tile) { return tile; }
	}
	return nullptr;
}

//sound named by "Sound <name>" in the .material file, which is a file in Assets/SoundsFiles.
//materials that declare none keep using their own name. "" means no material sound.
std::string S_Sound::GetMaterialSound(const Materials::MaterialType& i_material) {
	const std::string name = Materials::MaterialToString(i_material);
	if (name == "") { return ""; }
	auto material = m_materials.find(name);
	if (material == m_materials.end()) { return ""; }
	const std::vector<std::string>& sounds = material->second.m_soundNames;
	if (sounds.empty()) { return name; }
	return sounds[Utils::RandomIndex(sounds.size())];
}

sf::Vector3f S_Sound::MakeSoundPosition(const sf::Vector2f& i_pos, unsigned int i_elevation){
	return sf::Vector3f(i_pos.x, i_elevation * Sheet::Tile_Size, i_pos.y);
}

void S_Sound::EmitSound(const EntityId& i_entity, const EntitySound& i_sound, bool i_useId, bool i_relative
	, int i_checkFrame, const std::string& i_overrideSound) {
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
		if (i_overrideSound == "") {
			c_sound->SetSoundId(m_soundManager->Play(c_sound->GetSound(i_sound), pos));
		}
		else
		{
			c_sound->SetSoundId(m_soundManager->Play(i_overrideSound,pos, false,i_relative));
		}
		
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
