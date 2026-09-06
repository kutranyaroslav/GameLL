#pragma once
#include "C_Drawable.h"
#include <string>
#include "SpriteSheet.h"
class C_SpriteSheet: public C_Drawable
{
public:
	C_SpriteSheet();
	~C_SpriteSheet();
	virtual void ReadIn(std::stringstream& i_stream)override;
	virtual const sf::Vector2f GetSize() override;
	virtual void UpdatePosition(const sf::Vector2f& i_vec) override;
	virtual void Draw(sf::RenderTarget* i_wind) override;
	void Create(TextureManager* i_textureMgr, const std::string& i_name = "");
	SpriteSheet* GetSpriteSheet();
private:
	SpriteSheet* m_spriteSheet;
	std::string m_sheetName;
};

