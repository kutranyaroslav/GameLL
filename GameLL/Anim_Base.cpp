#include "Anim_Base.h"
#include "SpriteSheet.h"
#include "Anim_Directional.h"


Anim_Base::Anim_Base(): m_currentFrame(0),m_startFrame(0),
m_endFrame(0),m_rowFrame(0),m_frameTime(0.f),m_elapsedTime(0.f),
m_frameActionStart(-1),m_frameActionEnd(-1), m_loop(false), m_playing(false)
{}
Anim_Base::~Anim_Base(){}

void Anim_Base::SetSpriteSheet(SpriteSheet* i_spriteSheet) {
	m_spriteSheet = i_spriteSheet;
}

void Anim_Base::SetFrame(const unsigned int& i_frame) {
	if ((i_frame >= m_startFrame && i_frame <= m_endFrame) || (i_frame >= m_endFrame && i_frame <= m_startFrame)) {
		m_currentFrame = i_frame;
	}
}
void Anim_Base::SetName(const std::string& i_name) {
	m_name = i_name;
}
std::string Anim_Base::GetName() { return m_name; }
bool Anim_Base::IsInAction() {
	if (m_frameActionStart == -1 || m_frameActionEnd == -1) {
		return true;
	}
	return (m_currentFrame >= m_frameActionStart && m_currentFrame <= m_frameActionEnd);
}
bool Anim_Base::IsPlaying() { return m_playing; }
void Anim_Base::SetLooping(const bool& i_loop) {
	m_loop = i_loop;
}
void Anim_Base::Play() { m_playing = true; }
void Anim_Base::Pause() { m_playing = false; }
void Anim_Base::Stop() { m_playing = false; Reset(); }

void Anim_Base::Reset() {
	m_currentFrame = m_startFrame;
	m_elapsedTime = 0.0f;
	CropSprite();
}

void Anim_Base::Update(const sf::Time& i_dT) {
	if (!m_playing) { return; }
	m_elapsedTime += i_dT.asSeconds();
	if (m_elapsedTime < m_frameTime) { return; }
	FrameStep();
	CropSprite();
	m_elapsedTime = 0;
}