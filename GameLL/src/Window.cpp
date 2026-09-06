#include "Window.h"
#include "GUI_Manager.h"	
namespace {
	sf::FloatRect GetLetterboxViewport(const sf::View& i_view, const sf::Vector2u& i_windowSize) {
		float windowRatio = i_windowSize.x / (float)i_windowSize.y;
		float viewRatio = i_view.getSize().x / (float)i_view.getSize().y;
		float sizeX = 1.f, sizeY = 1.f, posX = 0.f, posY = 0.f;
		if (windowRatio > viewRatio) {
			sizeX = viewRatio / windowRatio;
			posX = (1.f - sizeX) / 2.f;
		}
		else {
			sizeY = windowRatio / viewRatio;
			posY = (1.f - sizeY) / 2.f;
		}
		return sf::FloatRect(posX, posY, sizeX, sizeY);
	}
}
Window::Window()
{
	Setup("Window", sf::Vector2u(1920, 1080));
	
}

Window::Window(const std::string& i_title, const sf::Vector2u& i_size) {
	Setup(i_title, i_size);
}

Window::~Window() { Destroy(); }

sf::FloatRect Window::GetSceneViewSpace()
{
	sf::Vector2f viewCenter = m_sceneTexture.getView().getCenter();
	sf::Vector2f viewSize = m_sceneTexture.getView().getSize();
	sf::Vector2f viewSizeHalf(viewSize.x / 2, viewSize.y / 2);
	sf::FloatRect viewSpace(viewCenter - viewSizeHalf, viewSize);
	return viewSpace;
}

void Window::Setup(const std::string& i_title, const sf::Vector2u& i_size) {
	m_windowTitle = i_title;
	m_windowedSize = i_size;   // remember the *preferred windowed* size separately
	m_windowSize = i_size;     // current/live size — kept in sync from here on
	m_isDone = false;
	m_isFullScreen = false;
	m_isFocused = true;
	m_eventManager.AddCallback(StateType(0), "Fullscreen_toggle", &Window::ToggleFullScreen, this);
	m_eventManager.AddCallback(StateType(0), "Window_close", &Window::Close, this);
	Create();
}
void Window::Create() {
	auto style = (m_isFullScreen ? sf::Style::Fullscreen : sf::Style::Default);
	sf::VideoMode mode = m_isFullScreen ? sf::VideoMode::getDesktopMode() :
		sf::VideoMode(m_windowedSize.x, m_windowedSize.y, 32);
	m_window.create(mode, m_windowTitle, style);
	OnResize(m_window.getSize());
}
void Window::OnResize(const sf::Vector2u& i_size)
{
	m_windowSize = i_size;
	m_sceneTexture.create(i_size.x, i_size.y);
	m_uiView.reset(sf::FloatRect(0, 0, (float)i_size.x, (float)i_size.y));
	//logic of all interfaces scaling 


	if (m_context && m_context->m_guiManager) {
		m_context->m_guiManager->OnResize(i_size);
	}
	// After the GUI, never before: state layout code reads the already-rescaled
	// element sizes and positions.
	if (m_context && m_context->m_stateManager) {
		m_context->m_stateManager->OnResize(i_size);
	}
	m_gameView.setViewport(GetLetterboxViewport(m_gameView, i_size));
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
		else if (event.type == sf::Event::Resized) {
			OnResize({ event.size.width, event.size.height });
		}
 		m_eventManager.HandleEvent(event);
		
	}
	m_eventManager.Update();
}

sf::RenderWindow* Window::GetRenderWindow() {
	return &m_window;
}
void Window::ToggleFullScreen(EventDetails* i_details) {
	m_isFullScreen = !m_isFullScreen;
	Destroy();
	Create();
}
void Window::BeginDraw() { 
	m_window.clear(sf::Color::Black);
	m_sceneTexture.clear(sf::Color::Black);
}
void Window::EndDraw() { m_window.display(); }
void Window::DisplayScene()
{
	m_sceneTexture.display();
	sf::Sprite full(m_sceneTexture.getTexture());
	//potentially maybe will a bug related to view
	m_window.setView(m_window.getDefaultView());
	m_window.draw(full);
}
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
sf::FloatRect Window::GetGameViewSpace() {
	sf::Vector2f viewCenter = m_gameView.getCenter();
	sf::Vector2f viewSize = m_gameView.getSize();
	return sf::FloatRect(viewCenter - viewSize / 2.f, viewSize);
}

sf::Vector2u& Window::GetWindowedSize()
{
	return m_windowedSize;
}

void Window::SetGameViewSize(const sf::Vector2f& i_size)
{
	m_gameView.setSize(i_size);
	OnResize(m_windowSize);
}

void Window::SetSharedContext(SharedContext* i_context)
{
	m_context= i_context;
}

SharedContext* Window::GetSharedContext() {
	return m_context;
}

bool Window::IsFocused()  { return m_isFocused; }