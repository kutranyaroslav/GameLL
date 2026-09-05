#pragma once
#include "TextureManager.h"
#include "SystemManager.h"
#include "EntityManagerNew.h"
#include "FontManager.h"
#include "dev/ErrorLogManager.h"
#include "Window.h"
class World;
class GUI_Manager;
class EntityManager;
class StateManager;
class AudioManager;
class SoundManager;

class Map;
class BaseState {
	friend class StateManager;
public:
	BaseState(StateManager* i_stateManager):
		m_stateManager(i_stateManager),
		m_transparent(false),
		m_transcendent(false){}

	virtual ~BaseState() {};
	virtual void onCreate() = 0;
	virtual void onDestroy() = 0;
	virtual void Activate() = 0;
	virtual void Deactivate() = 0; 
	virtual void Update(const sf::Time& i_time) = 0;
	virtual void Draw() = 0;

	void SetTransparent(const bool& i_transparent) {
		m_transparent = i_transparent;
	}
	bool IsTransparent()const { return m_transparent; }
	
	void SetTranscendent(const bool& i_transcendent) {
		m_transcendent = i_transcendent;
	}
	bool IsTranscendent() const { return m_transcendent; }
	StateManager* GetStateManager() { return m_stateManager; }
	StateType GetState() { return m_state; }
	sf::View* GetView() { return m_view; }


protected:
	StateType m_state;
	sf::View* m_view;
	StateManager* m_stateManager;
	std::vector<std::string> m_dynamicCallbacks;
	bool m_transparent;
	bool m_transcendent;
};


enum class StateType {
	Intro = 1, MainMenu, Game, Paused, GameOver, Credits, Developement
};
class StateManager;

struct SharedContext {
	SharedContext():m_wind(nullptr), m_eventManager(nullptr), m_textbox(nullptr), 
		m_textureManager(nullptr), m_stateManager(nullptr), m_entityManager(nullptr), 
		m_systemManager(nullptr),m_fontManager(nullptr), m_guiManager(nullptr),m_soundManager(nullptr), m_audioManager(nullptr),
	m_world(nullptr)
	{
		
	}
	
	Window* m_wind;
	EventManager* m_eventManager;
	Textbox* m_textbox;
	TextureManager* m_textureManager;
	StateManager* m_stateManager;
	EntityManagerNew* m_entityManager;
	SystemManager* m_systemManager;
	FontManager* m_fontManager;
	GUI_Manager* m_guiManager;
	SoundManager* m_soundManager;
	AudioManager* m_audioManager;
	ErrorLogManager* m_errorLogManager;
	World* m_world;
	
};

using StateContainer = std::vector<std::pair<StateType, BaseState*>>;
using TypeContainer = std::vector<StateType>;
using StateFactory = std::unordered_map<StateType, std::function<BaseState* (void)>>;

class StateManager {
public:
	StateManager(SharedContext* i_shared);
	~StateManager();

	void Update(const sf::Time& i_time);
	void Draw();
	void ProcessRequests();
	bool HasState(const StateType& i_type);
	void SwitchTo(const StateType& i_type);
	void Remove(const StateType& i_type);
	BaseState* GetState(StateType i_type);
	SharedContext* GetSharedContext();
	BaseState* GetCurrentState();
private:
	void CreateState(const StateType& i_type);
	void RemoveState(const StateType& i_type);

	template<class T>
	void RegisterState(const StateType& i_type){
		m_stateFactory[i_type] = [this]()->BaseState* {
			return new T(this);
			};
	}
	BaseState* m_currentState;
	SharedContext* m_shared;
	StateContainer m_states;
	TypeContainer m_toRemove;
	StateFactory m_stateFactory;
	
};

