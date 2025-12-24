#include "Window.h"

Window::Window() {
	Setup("Window", sf::Vector2u(1980, 1080));
	
}

Window::Window(const std::string& i_title, const sf::Vector2u& i_size) {
	Setup(i_title, i_size);
}

Window::~Window() { Destroy(); }

void Window::Setup(const std::string& i_title, const sf::Vector2u& i_size) {
	m_windowTitle = i_title;
	m_windowSize = i_size;
	m_isDone = false;
	m_isFullScreen = false;
	m_isFocused = true;
	m_eventManager.AddCallback(StateType(0), "Fullscreen_toggle",
		&Window::ToggleFullScreen, this);
	m_eventManager.AddCallback(StateType(0), "Window_close",
		&Window::Close, this);
	Create();
}
void Window::Create() {
	auto style = (m_isFullScreen ? sf::Style::Fullscreen : sf::Style::Default);
	m_window.create({ m_windowSize.x, m_windowSize.y, 32 }, m_windowTitle, style);
}
void Window::Destroy() {
	m_window.close();
}
void Window::Update() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::LostFocus) {
			m_isFocused = false;
		}
		else if (event.type == sf::Event::GainedFocus) {
			m_isFocused = true;
			m_eventManager.setFocus(m_isFocused);
		}
 		m_eventManager.HandleEvent(event);
		
	}
	m_eventManager.Update();
	sf::View view = GetRenderWindow()->getView();
	sf::Vector2f viewSize = view.getSize();
	sf::Vector2f viewCenter = view.getCenter();
	sf::Vector2f viewTopLeft = viewCenter - (viewSize / 2.0f);
	m_textbox.Setup(5, 16, 200, viewTopLeft);
}

sf::RenderWindow* Window::GetRenderWindow() {
	return &m_window;
}
void Window::ToggleFullScreen(EventDetails* i_details) {
	m_isFullScreen = !m_isFullScreen;
	Destroy();
	Create();
}
void Window::BeginDraw() { m_window.clear(sf::Color::Black); }
void Window::EndDraw() { m_window.display(); }
bool Window::IsDone() { return m_isDone; }
bool Window::IsFullScreen() { return m_isFullScreen; }
sf::Vector2u Window::GetWindowSize() { return m_windowSize; }
Textbox* Window::GetTextbox() { return &m_textbox; }

void Window::Draw(sf::Drawable& i_drawable) {
	m_window.draw(i_drawable);
}

void Window::Close(EventDetails* i_details) {
	m_isDone = true;
}

EventManager* Window::GetEventManager() {
	return &m_eventManager;
}
sf::FloatRect Window::GetViewSpace() {
	sf::Vector2f viewCenter = m_window.getView().getCenter();
	sf::Vector2f viewSize = m_window.getView().getSize();
	sf::Vector2f viewSizeHalf(viewSize.x / 2, viewSize.y / 2);
	sf::FloatRect viewSpace(viewCenter - viewSizeHalf, viewSize);
	return viewSpace;
}

bool Window::IsFocused()  { return m_isFocused; }