#include "Anim_Directional.h"
#include "Anim_Base.h"
#include "SpriteSheet.h"
void Anim_Directional::CropSprite() {
	sf::IntRect rect(m_spriteSheet->GetSpriteSize().x * m_currentFrame,
		m_spriteSheet->GetSpriteSize().y * (m_rowFrame + (short)m_spriteSheet->GetSpriteDir()), 
		m_spriteSheet->GetSpriteSize().x , m_spriteSheet->GetSpriteSize().y);
	m_spriteSheet->CropSprite(rect);
}

void Anim_Directional::FrameStep() {

	Direction d = m_spriteSheet->GetSpriteDir();
	Frame EndFrameFromDirection = m_endFrameEast;
	switch (d) {
	case Direction::Right:
		EndFrameFromDirection = m_endFrameEast;
		break;
	case Direction::Left:
		EndFrameFromDirection = m_endFrameWest;
		break;
	case Direction::Up:
		EndFrameFromDirection = m_endFrameNorth;
		break;
	case Direction::Down:
		EndFrameFromDirection = m_endFrameSouth;
		break;
	}
	bool b = SetFrame(m_currentFrame + (m_startFrame <= EndFrameFromDirection ? 1 : -1));
	if (b) { return; }
	if (m_loop) { SetFrame(m_startFrame); }
	else { SetFrame(EndFrameFromDirection); Pause(); }
	
}

void Anim_Directional::ReadIn(std::stringstream& i_stream) {
	i_stream >> m_startFrame >> m_endFrameEast >> m_endFrameWest >> m_endFrameNorth>> m_endFrameSouth >> m_rowFrame >>
		m_frameTime >> m_frameActionStart >> m_frameActionEnd>> m_loop;
}