#include "GUI_Interface.h"
#include "GUI_Manager.h"


GUI_Interface::GUI_Interface(const std::string& i_name, GUI_Manager* i_guiMgr):
	GUI_Element(i_name,GUI_ElementType::Window, this), m_parent(nullptr),
	m_guiManager(i_guiMgr), m_movable(false), m_beingMoved(false),
	m_showTitleBar(false), m_focused(false), m_scrollHorizontal(0),
	m_scrollVertical(0), m_contentRedraw(true), m_controlRedraw(true)
{
	m_backdropTexture = new sf::RenderTexture();
	m_contentTexture = new sf::RenderTexture();
	m_controlTexture = new sf::RenderTexture();
}

GUI_Interface::~GUI_Interface() {
	delete m_backdropTexture;
	delete m_contentTexture;
	delete m_controlTexture;
	for (auto& itr : m_elements) {
		delete itr.second;
	}
}
void GUI_Interface::SetPosition(const sf::Vector2f& i_pos) {
	GUI_Element::SetPosition(i_pos);
	m_backdrop.setPosition(i_pos);
	m_content.setPosition(i_pos);
	m_control.setPosition(i_pos);
	m_titleBar.setPosition(m_position.x,m_position.y - m_titleBar.getSize().y );
	m_visual.m_text.setPosition(m_titleBar.getPosition() + m_styles[m_state].m_textPadding);
}

GUI_Element* GUI_Interface::GetElement(const std::string& i_name)const {
	auto itr = m_elements.find(i_name);
	if (itr == m_elements.end()) { return nullptr; }
	return itr->second;
}

bool GUI_Interface::AddElement(const GUI_ElementType& i_type, const std::string& i_name) {
	auto itr = m_elements.find(i_name);
	if (itr != m_elements.end()) { return false; }
	GUI_Element* element = m_guiManager->CreateElement(i_type, this);
	if (!element) { return false; }
	element->SetOwner(this);
	element->SetName(i_name);
	m_elements.emplace(i_name, element);
	m_contentRedraw = true;
	m_controlRedraw = true;
	return true;
	
}
bool GUI_Interface::RemoveElement(const std::string& i_name){
	auto itr = m_elements.find(i_name);
	if (itr == m_elements.end()) { return false; }
	delete itr->second;
	m_elements.erase(itr);
	m_contentRedraw = true;
	m_contentRedraw = true;
	AdjustContentSize();
	return true;
}
bool GUI_Interface::IsInside(const sf::Vector2f& i_point) {
	if (GUI_Element::IsInside(i_point)) { return true; }
	return m_titleBar.getGlobalBounds().contains(i_point);
}

void GUI_Interface::ReadIn(std::stringstream& i_stream) {
	std::string movableState;
	std::string titleShow;
	std::string title;
	i_stream >> m_elementPadding.x >> m_elementPadding.y >> movableState >> titleShow;
	Utils::ReadQuotedString(i_stream, title);
	m_visual.m_text.setString(title);
	if (movableState == "Movable") { m_movable = true; }
	if (titleShow == "Title") { m_showTitleBar = true; }

}
void GUI_Interface::OnClick(const sf::Vector2f& i_mousePos) {
	DefocusTextFields();
	if (m_titleBar.getGlobalBounds().contains(i_mousePos) && m_movable && m_showTitleBar) {
		m_beingMoved = true;
	}
	else {
		GUI_Event event;
		event.m_type = GUI_EventType::Click;
		event.m_interface = m_name.c_str();
		event.m_element = "";
		event.m_clickCoordinates.x = i_mousePos.x;
		event.m_clickCoordinates.y = i_mousePos.y;
		m_guiManager->AddEvent(event);
		for (auto& itr : m_elements) {
			if (!itr.second->IsInside(i_mousePos)) { continue; }
			itr.second->OnClick(i_mousePos);
			event.m_element = itr.second->m_name.c_str();
			m_guiManager->AddEvent(event);
		}
		SetState(GUI_ElementState::Clicked);
	} 
}

void GUI_Interface::OnRelease() {
	GUI_Event event;
	event.m_type = GUI_EventType::Release;
	event.m_interface = m_name.c_str();
	event.m_element = "";
	m_guiManager->AddEvent(event);
	for (auto& itr : m_elements) {
		if (itr.second->GetState() != GUI_ElementState::Clicked) { continue; }
		itr.second->OnRelease();
		event.m_element = itr.second->GetName().c_str();
		m_guiManager->AddEvent(event);
	}
	SetState(GUI_ElementState::Neutral);
}
void GUI_Interface::OnHover(const sf::Vector2f& i_mousePos) {
	GUI_Event event; 
	event.m_type = GUI_EventType::Hover;
	event.m_interface = m_name.c_str();
	event.m_element = ""; 
	event.m_clickCoordinates.x = i_mousePos.x;
	event.m_clickCoordinates.y = i_mousePos.y;
	m_guiManager->AddEvent(event);
	SetState(GUI_ElementState::Focused);
}
void GUI_Interface::OnLeave() {
	GUI_Event event; 
	event.m_type = GUI_EventType::Leave;
	event.m_interface = m_name.c_str();
	event.m_element = "";
	m_guiManager->AddEvent(event);
	SetState(GUI_ElementState::Neutral);
}

void GUI_Interface::Update(float i_dT) {
	sf::Vector2f mousePos = sf::Vector2f(m_guiManager->GetSharedContext()->m_eventManager->GetMousePos(
		m_guiManager->GetSharedContext()->m_wind->GetRenderWindow()));
	if (m_beingMoved && m_moveMouseLast != mousePos) {
		sf::Vector2f difference = mousePos - m_moveMouseLast;
		m_moveMouseLast = mousePos;
		sf::Vector2f newPosition = m_position + difference; 
		SetPosition(newPosition);
	}
	for (auto& itr : m_elements) {
		if (itr.second->m_needsRedraw) {
			if (itr.second->IsControl()) { m_controlRedraw = true; }
			else { m_contentRedraw = true; }
		}
		if (!itr.second->GetActive()) { continue; }
		itr.second->Update(i_dT);
		if (m_beingMoved) { continue; }
		GUI_Event event;
		event.m_interface = m_name.c_str();
		event.m_element = itr.second->GetName().c_str();
		event.m_clickCoordinates.x = mousePos.x;
		event.m_clickCoordinates.y = mousePos.y;
		if(IsInside(mousePos)&& itr.second->IsInside(mousePos)&&
			!m_titleBar.getGlobalBounds().contains(mousePos)){
			if (itr.second->GetState() != GUI_ElementState::Neutral) { continue; }
			itr.second->OnHover(mousePos);
			event.m_type = GUI_EventType::Hover;
		}
		else if (itr.second->GetState() == GUI_ElementState::Focused) {
			itr.second->OnLeave();
			event.m_type = GUI_EventType::Leave;
		}
		m_guiManager->AddEvent(event);
	}
}
void GUI_Interface::Draw(sf::RenderTarget* i_target) {
	i_target->draw(m_backdrop);
	i_target->draw(m_content);
	i_target->draw(m_control);
	i_target->draw(m_visual.m_text);
	if (!m_showTitleBar) { return; }
	i_target->draw(m_titleBar);
	
}
void GUI_Interface::ApplyStyle() {
	GUI_Element::ApplyStyle();
	m_visual.m_backgroundSolid.setPosition(0.f, 0.f);
	m_visual.m_backgroundImage.setPosition(0.f, 0.f);
	m_titleBar.setSize(sf::Vector2f(m_styles[m_state].m_size.x, 16.f));
	m_titleBar.setPosition(m_position.x, m_position.y - m_titleBar.getSize().y);
	m_titleBar.setFillColor(m_styles[m_state].m_elementColor);
	m_visual.m_text.setPosition(m_titleBar.getPosition() + m_styles[m_state].m_textPadding);
	m_visual.m_glyph.setPosition(m_titleBar.getPosition() + m_styles[m_state].m_glyphPadding);
}

void GUI_Interface::OnTextEntered(const char& i_char) {
	for (auto& itr : m_elements) {
		if (itr.second->GetType() != GUI_ElementType::Textfield) { continue; }
		if (itr.second->GetState() != GUI_ElementState::Clicked) { continue; }
		if (i_char == 8) {
			//backspace
			const auto& text = itr.second->GetText();
			itr.second->SetText(text.substr(0, text.length() - 1));
			return;
		}
		if (i_char < 32 || i_char > 126) { return; }
		std::string text = itr.second->GetText();
		text.push_back(i_char);
		itr.second->SetText(text);
	}
}
void GUI_Interface::BeginMoving() {
	if (!m_movable ||!m_showTitleBar ) { return; }
	m_beingMoved = true;
	SharedContext* context = m_guiManager->GetSharedContext();
	m_moveMouseLast = sf::Vector2f(context->m_eventManager->GetMousePos(context->m_wind->GetRenderWindow()));
}
void GUI_Interface::StopMoving() { m_beingMoved = false; }

void GUI_Interface::Redraw() {
	if (m_backdropTexture->getSize().x != m_styles[m_state].m_size.x ||
		m_backdropTexture->getSize().y != m_styles[m_state].m_size.y) {
		m_backdropTexture->create(m_styles[m_state].m_size.x, m_styles[m_state].m_size.y);
	}
	m_backdropTexture->clear(sf::Color(0, 0, 0, 0));
	ApplyStyle();
	m_backdropTexture->draw(m_visual.m_backgroundSolid);
	if (m_styles[m_state].m_backgroundImage != "") {
		m_backdropTexture->draw(m_visual.m_backgroundImage);
	}
	m_backdropTexture->display();
	m_backdrop.setTexture(m_backdropTexture->getTexture());
	m_backdrop.setTextureRect(sf::IntRect(0, 0, m_styles[m_state].m_size.x, m_styles[m_state].m_size.y));
	SetRedraw(false);
}
void GUI_Interface::RedrawContent() {
	if (m_contentTexture->getSize().x != m_contentSize.x ||
		m_contentTexture->getSize().y != m_contentSize.y) {
		m_contentTexture->create(m_contentSize.x, m_contentSize.y);
	}
	m_contentTexture->clear(sf::Color(0, 0, 0, 0));
	for (auto itr : m_elements) {
		GUI_Element* element = itr.second;
		if (!element->GetActive() || element->IsControl()) { continue; }
		element->ApplyStyle();
		element->Draw(m_contentTexture);
		element->SetRedraw(false);
	}
	m_contentTexture->display();
	m_content.setTexture(m_contentTexture->getTexture());
	m_content.setTextureRect(sf::IntRect(m_scrollHorizontal,m_scrollVertical,m_styles[m_state].m_size.x
	, m_styles[m_state].m_size.y));
	m_contentRedraw = false;
}
void GUI_Interface::RedrawControls() {
	if (m_controlTexture->getSize().x != m_styles[m_state].m_size.x ||
		m_controlTexture->getSize().y != m_styles[m_state].m_size.y) {
		m_controlTexture->create(m_styles[m_state].m_size.x, m_styles[m_state].m_size.y);
	}
	m_controlTexture->clear(sf::Color(0, 0, 0, 0));
	for (auto itr : m_elements) {
		GUI_Element* element = itr.second; 
		if (!element->GetActive() || !element->IsControl()) { continue; }
		element->ApplyStyle();
		element->Draw(m_controlTexture);
		element->SetRedraw(false);
	}
	m_controlTexture->display();
	m_control.setTexture(m_controlTexture->getTexture());
	m_control.setTextureRect(sf::IntRect(0, 0, m_styles[m_state].m_size.x, m_styles[m_state].m_size.y));
	m_controlRedraw = false;
}
void GUI_Interface::UpdateScrollHorizontal(unsigned int i_percent) {
	if (i_percent > 100) { return; }
	m_scrollHorizontal = ((m_contentSize.x - GetSize().x) / 100) * i_percent;
	sf::IntRect rect = m_content.getTextureRect();
	m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical, rect.width, rect.height));
}
void GUI_Interface::UpdateScrollVertical(unsigned int i_percent) {
	if (i_percent > 100) { return; }
	m_scrollVertical = ((m_contentSize.y - GetSize().y) / 100) * i_percent;
	sf::IntRect rect = m_content.getTextureRect(); 
	m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollHorizontal, rect.width, rect.height));
}

void GUI_Interface::AdjustContentSize( GUI_Element* i_reference) {
	if (i_reference) {
		sf::Vector2f bottomRight = i_reference->GetPosition() + i_reference->GetSize();
		if (bottomRight.x > m_contentSize.x) {
			m_contentSize.x = bottomRight.x;
			m_controlRedraw = true;
		}
		if (bottomRight.y > m_contentSize.y) {
			m_contentSize.y = bottomRight.y;
			m_controlRedraw = true;
		}
		return;
	}
	sf::Vector2f farthest = GetSize();
	for (auto& itr : m_elements) {
		GUI_Element* element = itr.second;
		if (!element->GetActive() || element->IsControl()) { continue; }
		sf::Vector2f bottomRight = element->GetPosition() + element->GetSize();
		if (bottomRight.x > farthest.x) {
			farthest.x = bottomRight.x;
			m_controlRedraw = true;
		}
		if (bottomRight.y > farthest.y) {
			farthest.y = bottomRight.y;
			m_controlRedraw = true;
		}
	}
	SetContentSize(farthest);
}

void GUI_Interface::SetContentSize(const sf::Vector2f& i_vec) { m_contentSize = i_vec; }

sf::Vector2f& GUI_Interface::GetGlobalPosition()const {
	sf::Vector2f position = m_position;
	GUI_Interface* i = m_parent;
	while (i) {
		position += i->GetPosition();
		i = i->m_parent;
	}
	return position;
}
sf::Vector2f GUI_Interface::GetPadding() { return m_elementPadding; }
sf::Vector2f GUI_Interface::GetContentSize() { return m_contentSize; }
GUI_Manager* GUI_Interface::GetGuiManager() { return m_guiManager; }
bool GUI_Interface::IsBeingMoved() { return m_beingMoved; }
bool GUI_Interface::IsFocused() { return m_focused; }
void GUI_Interface::Focus() { m_focused = true; }
bool GUI_Interface::GetContentRedraw() { return m_contentRedraw; }
bool GUI_Interface::GetControlRedraw() { return m_controlRedraw; }

void GUI_Interface::DefocusTextFields() {
	GUI_Event event;
	event.m_type = GUI_EventType::Release;
	event.m_interface = m_name.c_str();
	event.m_element = "";
	for (auto& itr : m_elements) {
		if (itr.second->GetType() != GUI_ElementType::Textfield) { continue; }
		itr.second->SetState(GUI_ElementState::Neutral);
		event.m_element = itr.second->GetName().c_str();
		m_guiManager->AddEvent(event);
	}
}

