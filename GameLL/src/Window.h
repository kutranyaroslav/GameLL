#pragma once
#include "Textbox.h"
#include "EventManager.h"
#include "PostProcessor.h"
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
	void DisplayScene();
	void Update();
	void ToggleFullScreen(EventDetails* i_details);
	void Close(EventDetails* i_details = nullptr);
	void Draw(sf::Drawable& i_drawable);

	void SetGameViewSize(const sf::Vector2f& i_size);
	void SetSharedContext(SharedContext* i_context);
	void SetVignetteStrength(float i_v) { m_vignetteStrength = i_v; }
	void SetGrainStrength(float i_v) { m_grainStrength = i_v; }
	void SetAberrationStrength(float i_v) { m_aberrationStrength = i_v; }
	void SetScanlineStrength(float i_v) { m_scanlineStrength = i_v; }
	void SetColorGrade(const sf::Glsl::Vec3& i_lift, const sf::Glsl::Vec3& i_gamma, const sf::Glsl::Vec3& i_gain) {
		m_lift = i_lift; m_gamma = i_gamma; m_gain = i_gain;
	}
	void SetBloomThreshold(float i_v) { m_bloomThreshold = i_v; }
	void SetBloomIntensity(float i_v) { m_bloomIntensity = i_v; }

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
	sf::RenderTexture* GetSceneTexture() { return &m_sceneTexture; }	
	sf::FloatRect GetSceneViewSpace();

private:
	void Setup(const std::string& i_title, const sf::Vector2u& i_size);
	void Destroy();
	void Create();
	void OnResize(const sf::Vector2u& i_size);
	void LoadShaders();

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
	//Scenes 
	sf::RenderTexture m_sceneTexture;

	sf::Shader* m_compositeShader = nullptr;
	bool m_compositeReady = false;
	sf::Clock m_fxClock;
	float m_vignetteStrength = 0.6f;
	float m_grainStrength = 0.10f;

	float m_aberrationStrength = 0.009f;
	float m_scanlineStrength = 0.5f;
	sf::Glsl::Vec3 m_lift{ 0.f, 0.f, 0.f };
	sf::Glsl::Vec3 m_gamma{ 1.f, 1.f, 1.f };
	sf::Glsl::Vec3 m_gain{ 1.f, 1.f, 1.f };

	PostProcessor m_postProcessor;
	float m_bloomThreshold = 0.1f;
	float m_bloomIntensity = 0.2f;

};

