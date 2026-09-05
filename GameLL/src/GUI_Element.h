#pragma once
#include <string>
#include <unordered_map>
#include "SFML/Graphics.hpp"
#include "TextureManager.h"
#include "World.h"


enum class GUI_ElementState { Neutral, Focused, Clicked};
enum class GUI_ElementType { None, Window, Label, Button, Scrollbar, Textfield,Tileset,Viewport };
class GUI_Manager;

struct GUI_Style {
	GUI_Style() :m_textSize(12), m_baseTextSize(12), m_textCenterOrigin(false),m_backgroundImageFullElement(false), m_backgroundImageColor(255, 255, 255, 255) {
		sf::Color none = sf::Color(0, 0, 0, 0);
		m_backgroundColor = none;
		m_elementColor = none;
		m_textColor = none;
	}
	//numerable data
	sf::Vector2f m_size;
	unsigned int m_textSize;
	// The authored size from the .style file, i.e. the size at scale 1. m_textSize
	// is always recomputed from this rather than scaled step by step.
	unsigned int m_baseTextSize;
	// Percentage each m_size axis was authored with, or negative when the axis was
	// authored in absolute pixels. Percentage axes are re-resolved against the
	// current reference on every resize instead of scaling a stale pixel value.
	sf::Vector2f m_sizePercent{ -1.f, -1.f };
	sf::Vector2f m_textPadding;
	sf::Vector2f m_glyphPadding;
	sf::Vector2f m_margin;
	// for slider
	sf::Vector2f m_elementSize; 

	sf::Color m_backgroundColor;
	sf::Color m_elementColor;
	std::string m_backgroundImage;
	sf::Color m_backgroundImageColor;
	sf::Color m_textColor;
	std::string m_textFont;
	bool m_textCenterOrigin;
	bool m_backgroundImageFullElement;
	std::string m_glyph;
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
	virtual void CallbackSetup() = 0;

	virtual void OnResize(const sf::Vector2f& i_scale);
	// What a percentage size is measured against: the owning interface, or the
	// render window for a top-level interface.
	virtual sf::Vector2f GetStyleReference();
	//created to optimize redraw of dynamic elements like hovering and so on
	virtual void DrawOverlay(sf::RenderTarget* i_target) = 0;
	virtual void UpdateStyle(const GUI_ElementState& i_state, const GUI_Style& i_style);
	virtual void ApplyStyle();
	virtual void SetPosition(const sf::Vector2f& i_pos);
	// Percentage each position axis was authored with, or negative for absolute
	// pixels. Mirrors GUI_Style::m_sizePercent.
	void SetPositionPercent(const sf::Vector2f& i_percent);
	//setters
	void SetScale(const float& i_scale);
	void SetName(const std::string& i_name);
	void SetOwner(GUI_Interface* i_owner);
	void SetRedraw(const bool& i_redraw);
	void SetState(const GUI_ElementState& i_state);
	void SetContentSize(sf::Vector2f& i_size);
	void SetSize(const sf::Vector2f& i_size);
	void SetTextSize(const unsigned int& i_size);
	void SetWorkArea(float i_area);
	void SetActive(const bool& const i_acitve);
	void SetText(const std::string& i_string);
	void SetBackgroundImage(const std::string& i_texture);
	//getters
	float GetScale();
	std::string& GetName();
	GUI_Interface* GetOwner();
	bool GerRedraw();
	GUI_ElementState& GetState();
	sf::Vector2f& GetPosition();
	sf::Vector2f& GetMargin();
	sf::Vector2f& GetSize();
	sf::Vector2i GetBgImageSize();
	GUI_ElementType& GetType();
	const bool& GetActive()const;
	const std::string& GetText()const;
	const bool IsControl();
	bool IsInside(const sf::Vector2f& i_point);
	sf::Vector2f GetGlobalPosition();
	sf::RectangleShape& GetSlider();
	//only for viewport to get access to the world class
	void SetWorld(World* i_world);
	World* GetWorld();
	void SetWindow(Window* i_wind);
	Window* GetWindow();
	void SetTextureManager(TextureManager* i_textureManager);
	TextureManager* GetTextureManager();
	void SetZoomLevel();
	float GetZoomLevel();
	void SetLayer(const int& i_layer);
	int GetLayer();
	void SetContext(SharedContext* i_context);
	SharedContext* GetContext();
	virtual void ApplyBgStyle();

	friend std::stringstream& operator>>(std::stringstream& i_stream, GUI_Element& b) {
		b.ReadIn(i_stream);
		return i_stream;
	}
protected:
	virtual void ApplyTextStyle();
	virtual void ApplyGlyphStyle();
	virtual void RequireTexture(const std::string& i_name);
	virtual void RequireFont(const std::string& i_name);
	virtual void ReleaseTexture(const std::string& i_name);
	virtual void ReleaseFont(const std::string& i_name);
	// m_baseTextSize scaled to the current m_scale, clamped to at least 1.
	unsigned int ScaleTextSize(unsigned int i_base) const;
	void ReleaseResources();

	std::string m_name;
	sf::Vector2f m_position;
	sf::Vector2f m_positionPercent{ -1.f, -1.f };
	ElementStyles m_styles;
	GUI_Visual m_visual;
	GUI_ElementType m_type;
	GUI_ElementState m_state;
	sf::RectangleShape m_slider;
	GUI_Interface* m_owner;
	// Set later via the SetX() methods (only the viewport/tileset need most of
	// them), so they must start null rather than indeterminate.
	World* m_world = nullptr;
	Window* m_wind = nullptr;
	TextureManager* m_textureManager = nullptr;


	float m_scale;
	//needed for viewport
	SharedContext* m_context = nullptr;
	float m_zoomLevel = 1.f;
	float m_workArea = 0.f;
	int m_layerIndex = 0;
	sf::View m_view;

	bool m_needsRedraw;
	bool m_active;
	bool m_isControl;
};