#pragma once
#include "GUI_Element.h"
#include "GUI_Events.h"
#include "Utilitites.h"


using Elements = std::unordered_map<std::string, GUI_Element*>;

class GUI_Manager;

class GUI_Interface:public  GUI_Element
{
	friend class GUI_Element;
	friend class GUI_Manager;
public:
	GUI_Interface(const std::string& i_name, GUI_Manager* i_guiMgr);
	~GUI_Interface();
	void SetPosition(const sf::Vector2f& i_pos);
	bool AddElement(const GUI_ElementType& i_type, const std::string& i_name);
	bool RemoveElement(const std::string& i_name);
	bool IsInside(const sf::Vector2f& i_point);

	void ReadIn(std::stringstream& i_stream) override;
	void OnClick(const sf::Vector2f& i_mousePos) override;
	void OnRelease() override;
	void OnHover(const sf::Vector2f& i_mousePos)override;
	void OnLeave() override;
	void Update(float i_dT) override;
	void Draw(sf::RenderTarget* i_target)override;
	void ApplyStyle()override;

	void OnTextEntered(const char& i_char);
	void BeginMoving();
	void StopMoving();
	void Redraw();
	void RedrawContent();
	void RedrawControls();
	void UpdateScrollHorizontal(unsigned int i_percent);
	void UpdateScrollVertical(unsigned int i_percent);
	void AdjustContentSize(GUI_Element* i_reference = nullptr);
	sf::Vector2f GetGlobalPosition();

	GUI_Element* GetElement(const std::string& i_name)const;
	sf::Vector2f GetContentSize();
	sf::Vector2f GetPadding();
	GUI_Manager* GetGuiManager();
	const Elements& GetElements();
	bool IsBeingMoved();
	bool IsFocused();
	bool GetContentRedraw();
	bool GetControlRedraw();
	void Focus();

	virtual void RequireTexture(const std::string& i_name) override;
	virtual void RequireFont(const std::string& i_name) override;
	virtual void ReleaseTexture(const std::string& i_name)override;
	virtual void ReleaseFont(const std::string& i_name)override;
	virtual void ApplyBgStyle() override;
	virtual void ApplyTextStyle() override;
	virtual void ApplyGlyphStyle() override;
private:
	void DefocusTextFields();
	void SetContentSize(const sf::Vector2f& i_vec);

	Elements m_elements;
	sf::Vector2f m_elementPadding;
	GUI_Interface* m_parent;
	GUI_Manager* m_guiManager;
	sf::RenderTexture* m_backdropTexture;
	sf::Sprite m_backdrop;

	sf::RectangleShape m_titleBar;
	sf::Vector2f m_moveMouseLast;
	bool m_showTitleBar;
	bool m_movable;
	bool m_beingMoved;
	bool m_focused;

	sf::RenderTexture* m_contentTexture;
	sf::Sprite m_content;
	sf::Vector2f m_contentSize;
	int m_scrollHorizontal;
	int m_scrollVertical;
	bool m_contentRedraw;

	sf::RenderTexture* m_controlTexture;
	sf::Sprite m_control;
	bool m_controlRedraw;

	sf::Text debugText;

};

