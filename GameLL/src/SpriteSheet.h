#pragma once
#include "StateManager.h"
#include "TextureManager.h"
#include "Anim_Base.h"
#include "Directions.h"
#include <vector>

using Animations = std::unordered_map<std::string, Anim_Base*>;

struct SpritePart {
	sf::IntRect rectangle;
	int layerOffset = 0;
};


class SpriteSheet
{
public:
	SpriteSheet(TextureManager* i_textManager);
	~SpriteSheet();
	bool LoadSheet(const std::string& i_file);
	bool SetAnimation(const std::string& i_name, const bool& i_play = false, const bool& i_loop = false);
	void ReleaseSheet();
	void CropSprite(const sf::IntRect& i_rect);
	void Update(const sf::Time& i_dT);
	void Draw(sf::RenderTarget* i_wind);
	void DrawOnLayer(sf::RenderTarget& i_wind,
		unsigned int i_entityLayer, unsigned int i_drawingLayer,bool useParts = true);
	void SetSpriteSize(const sf::Vector2i& i_size);
	void SetSpritePos(const sf::Vector2f& i_pos);
	void SetSpriteDir(const Direction& i_dir);
	sf::Vector2i GetSpriteSize();
	Direction GetSpriteDir();
	Anim_Base* GetCurrentAnim();
	sf::Sprite* GetSprite();
private:
	std::string m_texture;
	sf::Sprite m_sprite;
	sf::Vector2i m_spriteSize;
	sf::Vector2f m_spriteScale;
	Direction m_direction;
	std::string m_animType;
	Animations m_animations;
	Anim_Base* m_animationCurrent;
	TextureManager* m_textureManager;
	std::vector<SpritePart> m_parts;
};

