#include "Anim_Directional.h"
#include "Anim_Base.h"
#include "SpriteSheet.h"
void Anim_Directional::CropSprite() {
	if (m_spriteSheet->GetSpriteDir() == Direction::Left) {
		int x = m_rowFrame;
		int y = (int)m_spriteSheet->GetSpriteDir();
	}
	sf::IntRect rect(m_spriteSheet->GetSpriteSize().x * m_currentFrame,
		m_spriteSheet->GetSpriteSize().y * (m_rowFrame + (short)m_spriteSheet->GetSpriteDir()), 
		m_spriteSheet->GetSpriteSize().x , m_spriteSheet->GetSpriteSize().y);
	m_spriteSheet->CropSprite(rect);
}

void Anim_Directional::FrameStep() {
	if (m_startFrame < m_endFrame) { ++m_currentFrame; }
	else { --m_currentFrame; }
	if ((m_startFrame < m_endFrame && m_currentFrame > m_endFrame)
		|| (m_startFrame > m_endFrame && m_currentFrame < m_endFrame)) {
		if (m_loop) 
		{
			m_currentFrame = m_startFrame;
			return;
		}
		m_currentFrame = m_endFrame;
		Pause();
	}
}

void Anim_Directional::ReadIn(std::stringstream& i_stream) {
	i_stream >> m_startFrame >> m_endFrame >> m_rowFrame >> 
		m_frameTime >> m_frameActionStart >> m_frameActionEnd;
}