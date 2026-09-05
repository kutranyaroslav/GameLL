#include "EventManager.h"
#include "StateManager.h"
EventManager::EventManager()
{
	m_hasFocus = true;
	LoadBindings();
	
}

EventManager::~EventManager() {
	for (auto& itr : m_bindings) {
		delete itr.second;
		itr.second = nullptr;
	}
}
bool EventManager::AddBinding(Binding* i_binding) {
	if (m_bindings.find(i_binding->m_name) != m_bindings.end()) {
		return false;
	}
	else {
		return m_bindings.emplace(i_binding->m_name, i_binding).second;
	}
}
bool EventManager::RemoveBinding(std::string i_name) {
	auto itr = m_bindings.find(i_name);
	if (itr == m_bindings.end()) { return false; }
	delete itr->second;
	m_bindings.erase(itr);
	return true;
}
// EventManager.cpp
bool EventManager::AddDynamicBinding(const std::string& i_name, EventType i_type,
	const std::string& i_interface, const std::string& i_element)
{
	if (m_bindings.find(i_name) != m_bindings.end()) { return false; }

	Binding* bind = new Binding(i_name);

	GUI_Event guiEvent;
	guiEvent.m_interface = i_interface;
	guiEvent.m_element = i_element;

	bind->BindEvent(i_type, EventInfo(guiEvent));

	if (!AddBinding(bind)) { delete bind; return false; }
	return true;
}
//handling of main events 
void EventManager::HandleEvent(sf::Event& i_event) {
	for (auto& b_itr : m_bindings) {
		Binding* bind = b_itr.second;
		for (auto& e_itr : bind->m_events) {
			EventType sfmlEvent = (EventType)i_event.type;
			if (e_itr.first == EventType::GUI_Click || e_itr.first == EventType::GUI_Release ||
				e_itr.first == EventType::GUI_Hover || e_itr.first == EventType::GUI_Leave) {
				continue;
			}
			if (e_itr.first != sfmlEvent) { continue; }
			if (sfmlEvent == EventType::KeyDown || sfmlEvent == EventType::KeyUp) {
				if (e_itr.second.m_code == i_event.key.code) {
					if (bind->m_details.m_keyCode != -1) {
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
					break;
				}
			}
			else if (sfmlEvent == EventType::MButtonDown || sfmlEvent == EventType::MButtonUp) {
				if (e_itr.second.m_code == i_event.mouseButton.button) {
					bind->m_details.m_mouse.x = i_event.mouseButton.x;
					bind->m_details.m_mouse.y = i_event.mouseButton.y;
					if (bind->m_details.m_keyCode != -1) {
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
					break;
				}
			}
			else {
				if (sfmlEvent == EventType::MouseWheel) {
					bind->m_details.mouseWheelDelta = i_event.mouseWheel.delta;
				}
				else if (sfmlEvent == EventType::WindowResize) {
					bind->m_details.m_size.x = i_event.size.width;
					bind->m_details.m_size.y = i_event.size.height;
				}
				else if (sfmlEvent == EventType::TextEntered) {
					bind->m_details.m_textEntered = i_event.text.unicode;
				}
				++(bind->c);
				
			}
		}
	}
}

void EventManager::HandleEvent(GUI_Event& i_event)
{
	for (auto& b_itr : m_bindings) {
		Binding* b = b_itr.second;
		for (auto& e_itr : b->m_events) {
			EventType sfmlEvent = (EventType)i_event.m_type;
			if (e_itr.first != EventType::GUI_Click && e_itr.first != EventType::GUI_Release &&
				e_itr.first != EventType::GUI_Hover && e_itr.first != EventType::GUI_Leave) {
				continue;
			}
			if ((e_itr.first == EventType::GUI_Click && i_event.m_type != GUI_EventType::Click) ||
				(e_itr.first == EventType::GUI_Release && i_event.m_type != GUI_EventType::Release) ||
				(e_itr.first == EventType::GUI_Hover && i_event.m_type != GUI_EventType::Hover) ||
				(e_itr.first == EventType::GUI_Leave && i_event.m_type != GUI_EventType::Leave)) {
				continue;
			}
			if (e_itr.second.m_guiEvent.m_interface != i_event.m_interface ||
				e_itr.second.m_guiEvent.m_element != i_event.m_element) {
				continue;
			}
			b->m_details.m_guiInterface = i_event.m_interface;
			b->m_details.m_guiElement = i_event.m_element;
			++(b)->c;
		}
	}
}

void EventManager::Update() {
	if (!m_hasFocus) { return; }
	for (auto& b_itr : m_bindings) {
		Binding* bind = b_itr.second;
		for (auto& e_itr : bind->m_events) {
			switch (e_itr.first)
			{
			case EventType::Keyboard:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(e_itr.second.m_code))) {
					if (bind->m_details.m_keyCode != -1) {
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
				}
				break;
			case EventType::Mouse:
				if (sf::Mouse::isButtonPressed(sf::Mouse::Button(e_itr.second.m_code))) {
					if (bind->m_details.m_keyCode != -1) {
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
				}

			case EventType::Joystick:
				//up to expansion
				break;
			}
		}
		if (bind->m_events.size() == bind->c) {
			auto stateCallbacks = m_callbacks.find(m_currentState);
			auto otherCallbacks = m_callbacks.find(StateType(0));

			if (stateCallbacks != m_callbacks.end()) {
				auto callitr = stateCallbacks->second.find(bind->m_name);
				if (callitr != stateCallbacks->second.end()) {
					callitr->second(&bind->m_details);
				}
			}
			if (otherCallbacks != m_callbacks.end()) {
				auto callitr = otherCallbacks->second.find(bind->m_name);
				if (callitr != otherCallbacks->second.end()) {
					callitr->second(&bind->m_details);
				}
			}
		}
		bind->c = 0;
		bind->m_details.Clear();
	}
	
}
void EventManager::LoadBindings() {
	//TO DO Fix the problem of bindings GUI_EVENTS not being binded probebly cause code initialized only in else
	std::string delimiter = ":";
	std::ifstream bindings;
	bindings.open(Utils::GetWorkingDirectory() + "nav//" + "binding_config.cfg");
	if (!bindings.is_open()) {
		std::cout << "Something went wrong " << std::endl;
		return;
	}
	std::string line;
	while (std::getline(bindings, line)) {
		std::stringstream keystream(line);
		std::string callbackName;
		keystream >> callbackName;
		Binding* bind = new Binding(callbackName);
		while (!keystream.eof()) {
			std::string keyval;
			keystream >> keyval;
			int start = 0;
			int end = keyval.find(delimiter);
			if (end == std::string::npos) {
				std::cout << "delimiter not found " << std::endl;
				delete bind;
				bind = nullptr;
				break;
			}
			EventType event = EventType(stoi(keyval.substr(start, end)));
			EventInfo eventInfo;
			if (event == EventType::GUI_Click || event == EventType::GUI_Release ||
				event == EventType::GUI_Hover || event == EventType::GUI_Leave) {
				start = end + delimiter.length();
				end = keyval.find(delimiter, start);
				std::string window = keyval.substr(start, end - start);
				std::string element;
				if (end != std::string::npos) {
					start = end + delimiter.length();
					end = keyval.length();
					element = keyval.substr(start, end);
				}		
				eventInfo.m_guiEvent.m_interface = window;
				eventInfo.m_guiEvent.m_element = element;
			}
			else {
			int code = stoi(keyval.substr(end + delimiter.length(),
				keyval.find(delimiter, end + delimiter.length())));
			
			eventInfo.m_code = code;
			}

			try {
				EventType type = static_cast<EventType>(std::stoi(keyval.substr(0, end)));
				if (event != EventType::GUI_Click && event != EventType::GUI_Release &&
					event != EventType::GUI_Hover && event != EventType::GUI_Leave) {
					int code = std::stoi(keyval.substr(end + delimiter.length()));
					eventInfo.m_code = code;
				}
				bind->BindEvent(type, eventInfo);
			}
			catch (const cException& e) {
				THROW_EXCEPTION(100048, "Bind failed: - " + bind->m_name);
				if (m_context){
					m_context->m_errorLogManager->GetInstance()->LogException(e);
				}
				delete bind;
				bind = nullptr;
				break;
			}
		}
		if (bind) { if (!AddBinding(bind)) { delete bind; } }
		bind = nullptr;
		}
		bindings.close();
	}

void EventManager::setFocus(bool i_focus) {
	m_hasFocus = i_focus;
}
void EventManager::SetCurrentState(const StateType& i_type) {
	m_currentState = i_type;
}
void EventManager::SetContext(SharedContext* i_context) {
	m_context = i_context;
}