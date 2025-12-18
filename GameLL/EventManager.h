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
	EventInfo() { m_code = 0; }
	EventInfo(int i_code):m_code(i_code) {}
	EventInfo(GUI_Event i_guiEvent): m_guiEvent(i_guiEvent){}
	union 
	{
		int m_code;
		GUI_Event m_guiEvent;
	};
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
	~Binding() {
		for (auto itr = m_events.begin(); itr != m_events.end(); ++itr) {
			if (itr->first == EventType::GUI_Click || itr->first == EventType::GUI_Release ||
				itr->first == EventType::GUI_Hover || itr->first == EventType::GUI_Leave) {
				delete[] itr->second.m_guiEvent.m_interface;
				delete[] itr->second.m_guiEvent.m_element;
			}
		}
	}
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
private:
	void LoadBindings();

	Bindings m_bindings;
	Callbacks m_callbacks;
	StateType m_currentState;
	bool m_hasFocus = false;

};

