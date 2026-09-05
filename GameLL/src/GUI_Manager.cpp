#include "GUI_Manager.h"
#include <exception>
#include <string>

namespace {
	// .style files are hand-edited, so a malformed number must not terminate the
	// process.
	bool ParseStyleNumber(const std::string& i_text, double& o_value) {
		try {
			size_t consumed = 0;
			o_value = std::stod(i_text, &consumed);
			return consumed > 0;
		}
		catch (const std::exception&) { return false; }
	}

	// "120" is absolute pixels, "35%" is that fraction of i_reference. o_value is left
	// alone when the text does not parse, so the inherited style value stands.
	bool ResolveStyleLength(const std::string& i_text, float i_reference, float& o_value) {
		if (i_text.empty()) { return false; }
		if (i_text.find('%') == std::string::npos) {
			double absolute = 0.0;
			if (!ParseStyleNumber(i_text, absolute)) { return false; }
			o_value = static_cast<float>(absolute);
			return true;
		}
		std::string number = i_text;
		number.pop_back();
		double percent = 0.0;
		if (!ParseStyleNumber(number, percent)) { return false; }
		o_value = static_cast<float>(i_reference * (percent / 100.0));
		return true;
	}
}



GUI_Manager::GUI_Manager(EventManager* i_eventMgr, SharedContext* i_context):
m_eventMgr(i_eventMgr), m_context(i_context), m_currentState(StateType(0))
{
	RegisterElement<GUI_Label>(GUI_ElementType::Label);
	//RegisterElement<GUI_Button>(GUI_ElementType::Button);
	RegisterElement<GUI_Scrollbar>(GUI_ElementType::Scrollbar);
	RegisterElement<GUI_Textfield>(GUI_ElementType::Textfield);
	RegisterElement<GUI_Tileset>(GUI_ElementType::Tileset);
	RegisterElement<GUI_Viewport>(GUI_ElementType::Viewport);
	//RegisterElement<GUI_Window>(GUI_ElementType::Window);
	m_elemTypes.emplace("Label", GUI_ElementType::Label);
	m_elemTypes.emplace("Button", GUI_ElementType::Button);
	m_elemTypes.emplace("Scrollbar", GUI_ElementType::Scrollbar);
	m_elemTypes.emplace("Textfield", GUI_ElementType::Textfield);
	m_elemTypes.emplace("Window", GUI_ElementType::Window);
	m_elemTypes.emplace("Tileset", GUI_ElementType::Tileset);
	m_elemTypes.emplace("Viewport", GUI_ElementType::Viewport);

	m_eventMgr->AddCallback(StateType(0), "Mouse_Left", &GUI_Manager::HandleClick, this);
	m_eventMgr->AddCallback(StateType(0), "Mouse_Left_Release", &GUI_Manager::HandleRelease, this);
	m_eventMgr->AddCallback(StateType(0), "Text_Entered", &GUI_Manager::HandleTextEntered, this);
}


GUI_Manager::~GUI_Manager() {
	m_eventMgr->RemoveCallback(StateType(0), "Mouse_Left");
	m_eventMgr->RemoveCallback(StateType(0), "Mouse_Left_Release");
	m_eventMgr->RemoveCallback(StateType(0), "Text_Entered");
	for (auto& itr : m_interfaces) {
		for (auto& itr2 : itr.second) {
			delete itr2.second;
		}
	}
}
bool GUI_Manager::AddInterface(const StateType& i_state, const  std::string& i_name) {
	auto s = m_interfaces.emplace(i_state, GUI_Interfaces()).first;
	GUI_Interface* temp = new GUI_Interface(i_name, this);
	if (s->second.emplace(i_name, temp).second) {return true;}
	delete temp;
	return false;
}

GUI_Interface* GUI_Manager::GetInterface(const StateType& i_state, const std::string& i_name) {
	auto itr = m_interfaces.find(i_state);
	if (itr == m_interfaces.end()) { return nullptr; }
	auto itr2 = itr->second.find(i_name);
	return (itr2 != itr->second.end() ? itr2->second : nullptr);
	
}


bool GUI_Manager::RemoveInterface(const StateType& i_state, const std::string& i_name) {
	auto s = m_interfaces.find(i_state);
	if (s == m_interfaces.end()) { return false; }
	auto i = s->second.find(i_name);
	if (i == s->second.end()) { return false; }
	delete i->second;
	return s->second.erase(i_name);
}

void GUI_Manager::Update(float i_dT) {
	sf::Vector2i mousePxPos = m_eventMgr->GetMousePos(m_context->m_wind->GetRenderWindow());
	
	// convert from pixel space to view/world space
	sf::Vector2f mousePos = m_context->m_wind->GetRenderWindow()->mapPixelToCoords(mousePxPos);
	// now use mousePos (Vector2f) everywhere instead of sf::Vector2f(mousePos)
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	std::vector<std::pair<std::string ,GUI_Interface*>> ordered(state->second.begin(), state->second.end());
	for (auto itr = ordered.rbegin(); itr != ordered.rend(); ++itr) {
		GUI_Interface* i = itr->second;
		if (!i->GetActive()) { continue; }
		i->Update(i_dT);
		if (i->IsBeingMoved()) { continue; }
		if (i->IsInside(sf::Vector2f(mousePos))) {
			if (i->GetState() == GUI_ElementState::Neutral) {
				i->OnHover(sf::Vector2f(mousePos));
			}
			continue;
		}
		else if (i->GetState() == GUI_ElementState::Focused) {
			i->OnLeave();
		}
	}
	
}
void GUI_Manager::Draw(sf::RenderWindow* i_wind) {
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	for (auto& itr : state->second) {
		GUI_Interface* i = itr.second;
		if (!i->GetActive()) { continue; }
		if (i->GerRedraw()) { i->Redraw(); }
		if (i->GetContentRedraw()) { i->RedrawContent(); }
		if (i->GetControlRedraw()) { i->RedrawControls(); }
		i->Draw(i_wind);
	}
}

void GUI_Manager::HandleClick(EventDetails* i_details) {
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	sf::Vector2i mousePos = m_eventMgr->GetMousePos(m_context->m_wind->GetRenderWindow());
	std::vector<std::pair<std::string, GUI_Interface*>> ordered(state->second.begin(), state->second.end());
	for (auto itr = ordered.rbegin(); itr != ordered.rend(); ++itr) {
		if (!itr->second->IsInside(sf::Vector2f(mousePos))) { continue; }
		if (!itr->second->GetActive()) { continue; }
		// Exactly one interface may hold focus, otherwise HandleTextEntered cannot
		// tell which one should receive typing.
		for (auto& other : state->second) { other.second->Defocus(); }
		itr->second->OnClick(sf::Vector2f(mousePos));
		itr->second->Focus();
		if (itr->second->IsBeingMoved()) { itr->second->BeginMoving(); }
		return;
	}
}
void GUI_Manager::HandleRelease(EventDetails* i_details) {
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	for (auto& itr : state->second) {
		GUI_Interface* i = itr.second;
		if (!i->GetActive()) { continue; }
		if (i->GetState() == GUI_ElementState::Clicked) {
			i->OnRelease();
		}
		if (i->IsBeingMoved()) {
			i->StopMoving();
		}
	}
}
void GUI_Manager::HandleTextEntered(EventDetails* i_details) {
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	for (auto& itr : state->second) {
		if (!itr.second->GetActive()) { continue; }
		if (!itr.second->IsFocused()) { continue; }
		itr.second->OnTextEntered(i_details->m_textEntered);
		return;
	}
}

void GUI_Manager::SetCurrentState(const StateType& i_state) {
	if (m_currentState == i_state) { return; }
	m_currentState = i_state;
}

void GUI_Manager::AddEvent(GUI_Event i_event) {
	m_events[m_currentState].push_back(i_event);
}
bool GUI_Manager::PollEvent(GUI_Event& i_event) {
	if (m_events[m_currentState].empty()) { return false; }
	i_event = m_events[m_currentState].back();
	m_events[m_currentState].pop_back();
	return true;
}
SharedContext* GUI_Manager::GetSharedContext() { return m_context; }

GUI_Element* GUI_Manager::CreateElement(const GUI_ElementType& i_id, GUI_Interface* i_owner)
{
	if (i_id == GUI_ElementType::Window) {
		return new GUI_Interface("", this);
	}
	auto itr = m_factory.find(i_id);
	return (itr == m_factory.end() ? nullptr : itr->second(i_owner));
}

GUI_ElementType GUI_Manager::StringToType(const std::string& i_string)
{
	if (i_string == "Label") {
		return GUI_ElementType::Label;
	}
	else if (i_string == "Textfield") {
		return GUI_ElementType::Textfield; 
	}
	else if (i_string == "Scrollbar") {
		return GUI_ElementType::Scrollbar;
	}
	else if (i_string == "Window") {
		return GUI_ElementType::Window;
	}
	else if (i_string == "Button") {
		return GUI_ElementType::Button;
	}
	else if (i_string == "Tileset") {
		return GUI_ElementType::Tileset;
	}
	else if (i_string == "Viewport") {
		return GUI_ElementType::Viewport;
	}
	return GUI_ElementType::None;
}

bool GUI_Manager::LoadInterface(const StateType& i_state, const std::string& i_interface, const std::string& i_name)
{
	std::string InterfaceName;
	std::ifstream file; 
	std::string fullpath;
	fullpath = Utils::GetWorkingDirectory() + "Assets\\GUI\\Interfaces\\" + i_interface;
	file.open(fullpath);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			if (line[0] == '|') {	continue;	}
			std::stringstream keystream(line);
			std::string key; 
			keystream >> key;
			if (key == "Interface") {
				std::string style;
				keystream >> InterfaceName >> style;
				if (!AddInterface(i_state, i_name)) {
					return false;
				}
				GUI_Interface* i = GetInterface(i_state, i_name);
				keystream >> *i;
				if (!LoadStyle(style, i)) {
					return false;
				}
				i->SetContentSize(i->GetSize());
			}
			else if (key == "Element") {
				if (InterfaceName == "")
				{
					continue;
				}
				std::string type;
				std::string name;
				sf::Vector2f position;
				std::string style;
				keystream >> type >> name >> position.x >> position.y >> style;
				GUI_ElementType eType = StringToType(type);
				if (eType == GUI_ElementType::None) {
					continue;
				}
				GUI_Interface* i = GetInterface(i_state, i_name);
				if (!i) { continue; }
				if (!i->AddElement(eType, name)) { continue; }
				GUI_Element* e = i->GetElement(name);
				keystream >> *e;
				e->SetPosition(position);
				if (!LoadStyle(style, e)) { continue; }
			

			}
			

		}
		return true;
	}
	else {
		return false;
	}
	
}

void GUI_Manager::OnResize(const sf::Vector2u& i_size)
{
	float scaleX = (float)i_size.x /  GetSharedContext()->m_wind->GetWindowedSize().x; 
	float scaleY = (float)i_size.y / GetSharedContext()->m_wind->GetWindowedSize().y;
	for (auto itr = m_interfaces.begin(); itr != m_interfaces.end(); itr++) {
		for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2) {
			std::string interfaceName = itr2->first;
			GUI_Interface* i_interface = itr2->second; 
			if (i_interface) {
				i_interface->OnResize(sf::Vector2f(scaleX,scaleY));
			}
		}
	}

}

bool GUI_Manager::LoadStyle(const std::string& i_file, GUI_Element* i_element) {
	if (!i_element) { return false; }
	std::string currentState;
	GUI_Style parentStyle;
	GUI_Style temporaryStyle;
	std::ifstream file; 
	file.open(Utils::GetWorkingDirectory() + "Assets\\GUI\\Styles\\" + i_file);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			if (line[0] == '|') { continue; }
			std::stringstream keystream(line);
			std::string key; 
			keystream >> key; 
			if (key == "State") {
				if (currentState != "") { continue; }
				keystream >> currentState;	
			}
			else if (key == "/State") {
				if (currentState == "") { continue; }
				GUI_ElementState state = GUI_ElementState::Neutral;
				if (currentState == "Hover") { state = GUI_ElementState::Focused; }
				else if (currentState == "Clicked") { state = GUI_ElementState::Clicked; }
				if (state == GUI_ElementState::Neutral) {
					parentStyle = temporaryStyle;
					i_element->UpdateStyle(state, parentStyle);
					i_element->UpdateStyle(GUI_ElementState::Clicked, temporaryStyle);
					i_element->UpdateStyle(GUI_ElementState::Focused, temporaryStyle);
				}
				else {
					i_element->UpdateStyle(state, temporaryStyle);
				}
				temporaryStyle = parentStyle;
				currentState = "";
			}
			else {
				if (currentState == "") { continue; }
				if (key == "Size") {
					std::string numTypeX, numTypeY;
					keystream >> numTypeX;
					keystream >> numTypeY;
					// A percentage is relative to the owning interface, or to the window
					// when this element is itself a top-level interface.
					sf::Vector2f reference(
						static_cast<float>(this->GetSharedContext()->m_wind->GetRenderWindow()->getSize().x),
						static_cast<float>(this->GetSharedContext()->m_wind->GetRenderWindow()->getSize().y));
					if (i_element->GetOwner()) {
						reference = i_element->GetOwner()->GetSize();
					}
					ResolveStyleLength(numTypeX, reference.x, temporaryStyle.m_size.x);
					ResolveStyleLength(numTypeY, reference.y, temporaryStyle.m_size.y);
				}
				else if (key == "ElementSize") {
					std::string numTypeX, numTypeY;
					keystream >> numTypeX;
					keystream >> numTypeY;
					// ElementSize percentages are window-relative.
					ResolveStyleLength(numTypeX,
						static_cast<float>(this->GetSharedContext()->m_wind->GetRenderWindow()->getSize().x),
						temporaryStyle.m_elementSize.x);
					ResolveStyleLength(numTypeY,
						static_cast<float>(this->GetSharedContext()->m_wind->GetRenderWindow()->getSize().y),
						temporaryStyle.m_elementSize.y);
				}

				else if (key == "BgColor") {
					int r, g, b, a = 0;
					keystream >> r >> g >> b >> a;
					temporaryStyle.m_backgroundColor = sf::Color(r, g, b, a);
				}
				else if (key == "BgImage") {
					keystream >> temporaryStyle.m_backgroundImage;
					if (i_element->GetType() == GUI_ElementType::Tileset) {
						i_element->SetSize(sf::Vector2f(m_context->m_textureManager->GetResource
						(temporaryStyle.m_backgroundImage)->getSize()));

					}
				}
				else if (key == "Margin") {
					keystream >> temporaryStyle.m_margin.x >> temporaryStyle.m_margin.y;
				}
				else if (key == "BgImageColor") {
					int r, g, b, a = 0;
					keystream >> r >> g >> b >> a;
					temporaryStyle.m_backgroundImageColor = sf::Color(r, g, b, a);
				}
				else if (key == "BgFullElement") {
					temporaryStyle.m_backgroundImageFullElement = true;
				}
				else if (key == "TextColor") {
					int r, g, b, a = 0;
					keystream >> r >> g >> b >> a;
					temporaryStyle.m_textColor = sf::Color(r, g, b, a);
				}
				else if (key == "TextSize") {
					keystream >> temporaryStyle.m_textSize;
				}
				else if (key == "TextCenterOrigin") {
					temporaryStyle.m_textCenterOrigin = true;
				}
				else if (key == "TextPadding") {
					keystream >> temporaryStyle.m_textPadding.x >> temporaryStyle.m_textPadding.y;
				}
				else if (key == "Font") {
					keystream >> temporaryStyle.m_textFont;
				}
				else if (key == "ElementColor") {
					int r, g, b, a = 0;
					keystream >> r >> g >> b >> a;
					temporaryStyle.m_elementColor = sf::Color(r, g, b, a);
				}
				else if (key == "Glyph") {
					keystream >> temporaryStyle.m_glyph;
				}
				else if (key == "GlyphPadding") {
					keystream >> temporaryStyle.m_glyphPadding.x >> temporaryStyle.m_glyphPadding.y;
				}
				else if (key == "Position") {
					int a, b = 0;
					keystream >> a >> b;
					i_element->SetPosition(sf::Vector2f(a, b));
				}
				else {
					return false;
				}
			}
			
		}
		file.close();
		return true;
	}
	return false;
}