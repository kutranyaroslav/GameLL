#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <algorithm>
#include "GUI_Events.h"
#include "Utilitites.h"

struct SharedContext;



enum class EventType {
	KeyDown = sf::Event::KeyPressed,
	KeyUp = sf::Event::KeyReleased,
	MButtonDown = sf::Event::MouseButtonPressed,
	MButtonUp = sf::Event::MouseButtonReleased,
	MouseWheel = sf::Event::MouseWheelMoved,
	WindowResize = sf::Event::Resized,
	GainedFocus = sf::Event::GainedFocus,
	LostFocuse = sf::Event::LostFocus,
	MouseEntered = sf::Event::MouseEntered,
	MouseLeft = sf::Event::MouseLeft,
	Closed = sf::Event::Closed,
	TextEntered = sf::Event::TextEntered,
	Keyboard = sf::Event::Count + 1, Mouse, Joystick, GUI_Click,
	GUI_Release, GUI_Hover, GUI_Leave

};
struct EventInfo {
	EventInfo() : m_code(0) {}
	EventInfo(int i_code) : m_code(i_code) {}
	EventInfo(GUI_Event i_guiEvent) : m_code(0), m_guiEvent(i_guiEvent) {}
	// These were a union. GUI_Event owns std::strings now, which a raw union
	// cannot manage, and a key code and a GUI event never belong to the same
	// EventType, so holding both costs a few bytes and nothing else.
	int m_code;
	GUI_Event m_guiEvent;
};
struct EventDetails {
	EventDetails(const std::string& i_bindName):
		m_name(i_bindName)
	{
		Clear();
	}
	std::string m_name;
	sf::Vector2i m_size;
	sf::Uint32 m_textEntered;
	sf::Vector2i m_mouse;
	int mouseWheelDelta;
	int m_keyCode;
	std::string m_guiInterface;
	std::string m_guiElement;
	GUI_EventType m_guiEvent;
	void Clear() {
		m_size = sf::Vector2i(0, 0);
		m_textEntered = 0;
		m_mouse = sf::Vector2i(0, 0 );
		mouseWheelDelta = 0;
		m_keyCode = -1;
		m_guiInterface = "";
		m_guiElement = "";
		m_guiEvent = GUI_EventType::None;
	}
};

using Events = std::vector<std::pair<EventType, EventInfo>>;

struct Binding {
	Binding(const std::string& i_name):
		m_name(i_name), m_details(i_name), c(0)
	{}
	// No destructor needed: GUI_Event owns its strings now.
	bool BindEvent(EventType i_type, EventInfo i_eventInfo = EventInfo())
	{
		try
		{
			m_events.emplace_back(i_type, i_eventInfo);
			return true;
		}
		catch (const std::bad_alloc&) // in case memory allocation fails
		{
			return false;
		}
	}
	Events m_events;
	std::string m_name;
	int c;
	EventDetails m_details;
	
};

using Bindings = std::unordered_map<std::string, Binding*>;

using CallbackContainer = std::unordered_map<std::string, std::function<void(EventDetails*)>>;
enum class StateType;
using Callbacks = std::unordered_map<StateType, CallbackContainer>;

class EventManager {
public:
	EventManager();
	~EventManager();
	bool AddBinding(Binding* i_binding);
	bool RemoveBinding(std::string i_name);
	// EventManager.h - replace AddGUIBinding with this:
	bool AddDynamicBinding(const std::string& i_name, EventType i_type, const std::string& i_interface, const std::string& i_element);
	template <class T>
	bool AddCallback(StateType i_state, const std::string& i_name, void(T::*i_func) (EventDetails*), T* i_instance){
		auto itr = m_callbacks.emplace(i_state, CallbackContainer()).first;
		auto temp = std::bind(i_func, i_instance, std::placeholders::_1);
		return itr->second.emplace(i_name, temp).second;
	}
	bool RemoveCallback(StateType i_state, const std::string& name) {
		auto itr = m_callbacks.find(i_state);
		if (itr == m_callbacks.end()) { return false; }
		auto itr2 = itr->second.find(name);
		if (itr2 == itr->second.end()) { return false; }
		itr->second.erase(itr2);
		return true;
	}
	void HandleEvent(sf::Event& i_event);
	void HandleEvent(GUI_Event& i_event);
	void Update();
	sf::Vector2i GetMousePos(sf::RenderWindow* i_wind = nullptr) {
		return i_wind ? sf::Mouse::getPosition(*i_wind) : sf::Mouse::getPosition();
	}
	void setFocus(bool i_focus);
	void SetCurrentState(const StateType& i_type);
	void SetContext(SharedContext* i_contex);
private:
	void LoadBindings();
	SharedContext* m_context;
	Bindings m_bindings;
	Callbacks m_callbacks;
	StateType m_currentState;
	bool m_hasFocus = false;

};

