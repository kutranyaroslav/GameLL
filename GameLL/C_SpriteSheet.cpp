#include "C_SpriteSheet.h"

C_SpriteSheet::C_SpriteSheet():C_Drawable(Component::SpriteSheet),m_spriteSheet(nullptr) {}
C_SpriteSheet::~C_SpriteSheet() { if (m_spriteSheet) { delete m_spriteSheet; } }

void C_SpriteSheet::ReadIn(std::stringstream& i_stream) {
	i_stream >> m_sheetName;
}
void C_SpriteSheet::Create(TextureManager* i_textureMgr, const std::string& i_name) {
	if (m_spriteSheet) { return; }
	m_spriteSheet = new SpriteSheet(i_textureMgr);
	m_spriteSheet->LoadSheet("Media/SpriteSheets/" + (i_name != "" ? i_name : m_sheetName) + ".sheet");
}
void C_SpriteSheet::UpdatePosition(const sf::Vector2f& i_vec) {
	m_spriteSheet->SetSpritePos(i_vec);
}
const sf::Vector2f C_SpriteSheet::GetSize() {
	return (sf::Vector2f)m_spriteSheet->GetSpriteSize();
}

void C_SpriteSheet::Draw(sf::RenderWindow* i_wind) {
	if (!m_spriteSheet) { return; }
	m_spriteSheet->Draw(i_wind);
}
SpriteSheet* C_SpriteSheet::GetSpriteSheet() { return m_spriteSheet; }