#pragma once
#include <string>
#include <unordered_map>
#include "SFML/Graphics.hpp"
#include "TextureManager.h"



enum class GUI_ElementState { Neutral, Focused, Clicked };
enum class GUI_ElementType { None, Window, Label, Button, Scrollbar, Textfield };
class GUI_Manager;

struct GUI_Style {
	GUI_Style() :m_textSize(12), m_textCenterOrigin(false),m_backgroundImageFullElement(false), m_backgroundImageColor(255, 255, 255, 255) {
		sf::Color none = sf::Color(0, 0, 0, 0);
		m_backgroundColor = none;
		m_elementColor = none;
		m_textColor = none;
	}
	sf::Vector2f m_size;
	// for slider
	sf::Vector2f m_elementSize; 
	sf::Color m_backgroundColor;
	sf::Color m_elementColor;
	std::string m_backgroundImage;
	sf::Color m_backgroundImageColor;
	sf::Color m_textColor;
	std::string m_textFont;
	sf::Vector2f m_textPadding;
	unsigned int m_textSize;
	bool m_textCenterOrigin;
	bool m_backgroundImageFullElement;
	std::string m_glyph;
	sf::Vector2f m_glyphPadding;
	sf::Vector2f m_margin;
};
struct GUI_Visual {
	sf::RectangleShape m_backgroundSolid;
	sf::Sprite m_backgroundImage;
	sf::Sprite m_glyph;
	sf::Text m_text;
};

using ElementStyles = std::unordered_map<GUI_ElementState, GUI_Style>;

class GUI_Interface;

class GUI_Element {
	friend class GUI_Interface;
public:
	GUI_Element(const std::string& i_name, const GUI_ElementType& i_type, GUI_Interface* i_owner);
	virtual ~GUI_Element();

	virtual void ReadIn(std::stringstream& i_stream) = 0;
	virtual void OnClick(const sf::Vector2f& i_mousePos) = 0;
	virtual void OnRelease() = 0;
	virtual void OnHover(const sf::Vector2f& i_mousePos) = 0;
	virtual void OnLeave() = 0;
	virtual void Update(float i_dT) = 0;
	virtual void Draw(sf::RenderTarget* i_target) = 0;
	virtual void UpdateStyle(const GUI_ElementState& i_state, const GUI_Style& i_style);
	virtual void ApplyStyle();
	virtual void SetPosition(const sf::Vector2f& i_pos);

	void SetName(const std::string& i_name);
	std::string& GetName();
	void SetOwner(GUI_Interface* i_owner);
	GUI_Interface* GetOwner();
	void SetRedraw(const bool& i_redraw);
	bool GerRedraw();
	void SetState(const GUI_ElementState& i_state);
	GUI_ElementState& GetState();
	sf::Vector2f& GetPosition();
	sf::Vector2f& GetMargin();
	void SetContentSize(sf::Vector2f& i_size);
	sf::Vector2f& GetSize();
	GUI_ElementType& GetType();
	void SetActive(const bool& const i_acitve);
	const bool& GetActive()const;
	void SetText(const std::string& i_string);
	const std::string& GetText()const;
	const bool IsControl();
	bool IsInside(const sf::Vector2f& i_point);
	sf::Vector2f GetGlobalPosition();

	friend std::stringstream& operator>>(std::stringstream& i_stream, GUI_Element& b) {
		b.ReadIn(i_stream);
		return i_stream;
	}
protected:
	virtual void ApplyTextStyle();
	virtual void ApplyBgStyle();
	virtual void ApplyGlyphStyle();
	virtual void RequireTexture(const std::string& i_name);
	virtual void RequireFont(const std::string& i_name);
	virtual void ReleaseTexture(const std::string& i_name);
	virtual void ReleaseFont(const std::string& i_name);
	void ReleaseResources();

	std::string m_name;
	sf::Vector2f m_position;
	ElementStyles m_styles;
	GUI_Visual m_visual;
	GUI_ElementType m_type;
	GUI_ElementState m_state;
	GUI_Interface* m_owner;

	bool m_needsRedraw;
	bool m_active;
	bool m_isControl;
};