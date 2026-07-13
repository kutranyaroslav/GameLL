#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include "GUI_Interface.h"
#include "StateManager.h"
#include "GUI_Label.h"
#include "GUI_Textfield.h"
#include "GUI_Scrollbar.h"
#include "dev/GUI_Tileset.h"
#include "GUI_Viewport.h"

class EventManager;

enum class GUI_ElementType;


using GUI_Interfaces = std::unordered_map<std::string, GUI_Interface*>;
using GUI_Container = std::unordered_map<StateType, GUI_Interfaces>;
using GUI_Events = std::unordered_map<StateType, std::vector<GUI_Event>>;
using GUI_Factory = std::unordered_map<GUI_ElementType, std::function<GUI_Element*(GUI_Interface*)>>;
using GUI_ElemTypes = std::unordered_map<std::string, GUI_ElementType>;

class GUI_Manager
{
	friend class GUI_Interface;
public:
	GUI_Manager(EventManager* i_eventMgr, SharedContext* i_context);
	~GUI_Manager();

	bool AddInterface(const StateType& i_state,const std::string& i_name);
	GUI_Interface* GetInterface(const StateType& i_state, const std::string& i_name);
	bool RemoveInterface(const StateType& i_state, const std::string& i_name);
	void Update(float i_dT);
	void Draw(sf::RenderWindow* i_wind);

	template <class T>
	void RegisterElement(const GUI_ElementType& i_id) {
		m_factory[i_id] = [](GUI_Interface* i_owner)->GUI_Element* {
			return new T("", i_owner);
			};
	}
	void HandleClick(EventDetails* i_details);
	void HandleRelease(EventDetails* i_details);
	void HandleTextEntered(EventDetails* i_details);
	void SetCurrentState(const StateType& i_state);
	void AddEvent(GUI_Event i_event);
	bool PollEvent(GUI_Event& i_event);
	SharedContext* GetSharedContext();
	bool LoadStyle(const std::string& i_file, GUI_Element* i_element);
	bool LoadInterface(const StateType& i_state, const std::string& i_interface, const std::string& i_name);
private:
	GUI_Element* CreateElement(const GUI_ElementType& i_id, GUI_Interface* i_owner);
	GUI_ElementType StringToType(const std::string& i_string);
	

	GUI_Container m_interfaces;
	GUI_Events m_events;
	SharedContext* m_context;
	StateType m_currentState;
	GUI_Factory m_factory; 
	GUI_ElemTypes m_elemTypes;
	EventManager* m_eventMgr;
	
};

