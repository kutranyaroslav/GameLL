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
	Keyboard = sf::Event::Count + 1, Mouse, Joystick,
	
	
};

struct EventInfo {
	EventInfo() { m_code = 0; }
	EventInfo(int i_code):m_code(i_code) {}
	union 
	{
		int m_code;
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
	void Clear() {
		m_size = sf::Vector2i(0, 0);
		m_textEntered = 0;
		m_mouse = sf::Vector2i(0, 0 );
		mouseWheelDelta = 0;
		m_keyCode = -1;
	}
};

using Events = std::vector<std::pair<EventType, EventInfo>>;

struct Binding {
	Binding(const std::string& i_name):
		m_name(i_name), m_details(i_name), c(0)
	{

	}
	void BindEvent(EventType i_type, EventInfo i_eventInfo = EventInfo())
	{
		m_events.emplace_back(i_type, i_eventInfo);
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

