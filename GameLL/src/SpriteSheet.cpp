#include "SpriteSheet.h"
#include "Anim_Directional.h"
SpriteSheet::SpriteSheet(TextureManager* i_textureManager):
	m_textureManager(i_textureManager),m_animationCurrent(nullptr),
	m_spriteScale(1.0f, 1.0f), m_direction(Direction::Right)
{	
}
SpriteSheet::~SpriteSheet() { ReleaseSheet(); }

bool SpriteSheet::LoadSheet(const std::string& i_file) {
	std::ifstream sheet;
	sheet.open(Utils::GetWorkingDirectory() + i_file);
	if (sheet.is_open()) {
		ReleaseSheet();
		std::string line;
		while (std::getline(sheet, line)) {
			if (line[0] == '|') { continue; }
			std::stringstream keystream(line);
			std::string type;
			keystream >> type;
			if (type == "Texture") {
				if (m_texture != "") {
					std::cerr << "Duplicate texture entries in: " << i_file << "\n";
					continue;
				}
				std::string texture;
				keystream >> texture;
				if (!m_textureManager->RequireResource(texture)) {
					std::cerr << "Could not set up the texture: " << texture << "\n";
					continue;
				}
				m_texture = texture;
				if (!m_textureManager->GetResource(texture)) {
					
				}
				else {
					
				}
				m_sprite.setTexture(*m_textureManager->GetResource(m_texture));
			}
			else if(type == "Size") {
				keystream >> m_spriteSize.x >> m_spriteSize.y;
				SetSpriteSize(m_spriteSize);
			}
			else if (type == "Scale") {
				keystream >> m_spriteScale.x >> m_spriteScale.y;
				m_sprite.setScale(m_spriteScale);
			}
			else if (type == "AnimationType") {
				keystream >> m_animType;
			}
			else if (type == "Animation") {
				std::string name;
				keystream >> name;
				if (m_animations.find(name) != m_animations.end()) {
					std::cerr << "Duplicate Animation " << name << "\n";
					continue;
				}
				Anim_Base* anim = nullptr;
				if (m_animType == "Directional") {
					anim = new Anim_Directional();
				}
				else {//to be expanded 
					std::cerr << "unknown type of animations " << "\n ";
					continue;
				}
				keystream >> *anim;
				anim->SetSpriteSheet(this);
				anim->SetName(name);
				anim->Reset();
				m_animations.emplace(name, anim);
				if (m_animationCurrent) { continue;}
				m_animationCurrent = anim;
				m_animationCurrent->Play();
			}
		}
		sheet.close();
		return true;
	}
	std::cerr << "Failed oppening the file " << i_file << "\n";
	return false;
}
void SpriteSheet::ReleaseSheet() {
	m_textureManager->ReleaseResource(m_texture);
	m_animationCurrent = nullptr;
	while (m_animations.begin() != m_animations.end()) {
		delete m_animations.begin()->second;
		m_animations.erase(m_animations.begin());
	}
}
void SpriteSheet::SetSpriteSize(const sf::Vector2i& i_size) {
	m_spriteSize = i_size;
	
	m_sprite.setOrigin(m_spriteSize.x / 2, m_spriteSize.y / 2);
}
sf::Vector2i SpriteSheet::GetSpriteSize() {
	return m_spriteSize;
}
void SpriteSheet::SetSpritePos(const sf::Vector2f& i_pos) {
	m_sprite.setPosition(i_pos);
}
void SpriteSheet::SetSpriteDir(const Direction& i_dir) {
	if (m_direction == i_dir) { return; }
	m_direction = i_dir;
	/*if (m_animationCurrent->IsPlaying()) {
		m_animationCurrent->Stop(); 
	}
	m_animationCurrent->CropSprite();*/
}
Direction SpriteSheet::GetSpriteDir() {
	return m_direction;
}

void SpriteSheet::CropSprite(const sf::IntRect& i_rect) { 
	m_sprite.setTextureRect(i_rect);
}

bool SpriteSheet::SetAnimation(const std::string& i_name, const bool& i_play, const bool& i_loop) {
	auto itr = m_animations.find(i_name);
	if (itr == m_animations.end()) { return false; }
	if (itr->second == m_animationCurrent) { return false;}
	if (m_animationCurrent) { m_animationCurrent->Stop(); }
 	m_animationCurrent = itr->second;
	m_animationCurrent->SetLooping(i_loop);
	if (i_play) { m_animationCurrent->Play(); }
	m_animationCurrent->CropSprite();
	return true;
}

void SpriteSheet::Update(const sf::Time& i_dT) {
	if (m_animationCurrent) {
		m_animationCurrent->Update(i_dT);
	}
}

void SpriteSheet::Draw(sf::RenderTarget* i_wind) {
	i_wind->draw(m_sprite);
}
Anim_Base* SpriteSheet::GetCurrentAnim() {
	return m_animationCurrent;
}

sf::Sprite* SpriteSheet::GetSprite() { return &m_sprite; }