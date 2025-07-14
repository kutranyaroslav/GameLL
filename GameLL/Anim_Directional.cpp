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
	case Direction::Up:
		EndFrameFromDirection = m_endFrameNorth;
	case Direction::Down:
		EndFrameFromDirection = m_endFrameSouth;
	}
	if (m_startFrame < EndFrameFromDirection) { ++m_currentFrame; }
	else { --m_currentFrame; }
	if ((m_startFrame < EndFrameFromDirection && m_currentFrame >= EndFrameFromDirection)
		|| (m_startFrame > EndFrameFromDirection && m_currentFrame < EndFrameFromDirection)) {
		if (m_loop) 
		{
			m_currentFrame = m_startFrame;
			return;
		}
		m_currentFrame = m_endFrameEast;
		Pause();
	}
}

void Anim_Directional::ReadIn(std::stringstream& i_stream) {
	i_stream >> m_startFrame >> m_endFrameEast >> m_endFrameWest >> m_endFrameNorth>> m_endFrameSouth >> m_rowFrame >>
		m_frameTime >> m_frameActionStart >> m_frameActionEnd>> m_loop;
}