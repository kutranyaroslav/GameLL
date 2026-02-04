#pragma once
#include "Textbox.h"
#include "EventManager.h"
#include <string>


class Window
{
public:
	Window();
	Window(const std::string& i_title, const sf::Vector2u& i_size);
	~Window();
	bool IsFocused();
	bool IsDone();
	bool IsFullScreen();

	void BeginDraw();
	void EndDraw();
	void Update();
	void ToggleFullScreen(EventDetails* i_details);
	void Close(EventDetails* i_details = nullptr);
	void Draw(sf::Drawable& i_drawable);

	sf::Vector2u GetWindowSize();
	EventManager* GetEventManager();
	sf::RenderWindow* GetRenderWindow();
	Textbox* GetTextbox();
	sf::FloatRect GetViewSpace();
private:
	void Setup(const std::string& i_title, const sf::Vector2u& i_size);
	void Destroy();
	void Create();
	EventManager m_eventManager;
	Textbox m_textbox;
	sf::RenderWindow m_window;
	sf::Vector2u m_windowSize;
	std::string m_windowTitle;
	bool m_isDone = false;
	bool m_isFocused;
	bool m_isFullScreen;
};

