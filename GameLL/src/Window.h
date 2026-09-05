#pragma once
#include "Textbox.h"
#include "EventManager.h"
#include <string>

struct SharedContext;
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

	void SetGameViewSize(const sf::Vector2f& i_size);
	void SetSharedContext(SharedContext* i_context);
	SharedContext* GetSharedContext();
	sf::Vector2u GetWindowSize(); 
	EventManager* GetEventManager();
	sf::RenderWindow* GetRenderWindow();
	Textbox* GetTextbox();
	sf::FloatRect GetViewSpace();
	sf::FloatRect GetGameViewSpace();
	sf::Vector2u& GetWindowedSize();
	sf::View* GetUIView()    { return &m_uiView; }
	sf::View* GetGameView()  { return &m_gameView; }
private:
	void Setup(const std::string& i_title, const sf::Vector2u& i_size);
	void Destroy();
	void Create();
	void OnResize(const sf::Vector2u& i_size);
	EventManager m_eventManager;
	SharedContext* m_context = nullptr;
	Textbox m_textbox;
	sf::RenderWindow m_window;
	sf::Vector2u m_windowSize;
	std::string m_windowTitle;
	bool m_isDone = false;
	bool m_isFocused;
	bool m_isFullScreen;
	sf::Vector2u m_windowedSize; 
	sf::View m_uiView;
	sf::View m_gameView;
};

