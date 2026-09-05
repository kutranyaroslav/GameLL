#pragma once
#include <string>
enum GUI_EventType{None,Click, Release, Hover, Leave};

struct ClickCoordinates {
	float x, y;
};
struct GUI_Event {
	GUI_EventType m_type = GUI_EventType::None;
	// Owning copies. These used to be const char*: bindings pointed at heap char
	// arrays they freed themselves, while queued GUI events pointed into the
	// std::string of an element that could be removed before the event was polled
	// a frame later.
	std::string m_element;
	std::string m_interface;
	ClickCoordinates m_clickCoordinates{ 0.f, 0.f };
};