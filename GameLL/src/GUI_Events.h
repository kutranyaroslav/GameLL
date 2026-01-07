#pragma once
enum GUI_EventType{None,Click, Release, Hover, Leave};

struct ClickCoordinates {
	float x, y;
};
struct GUI_Event {
	GUI_EventType m_type;
	const char* m_element;
	const char* m_interface;
	union {
		ClickCoordinates m_clickCoordinates;
	};
};