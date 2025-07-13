#pragma once
#include <deque>
using EventId = unsigned int;

class EventQueue
{
public:
	void AddEvent(const EventId& i_event) { m_events.push_back(i_event); }
	void Clear() { while (!m_events.empty()) { m_events.pop_back();  } }
	bool ProcessEvents(EventId& i_id) {
		if (m_events.empty()) { return false; }
		i_id = m_events.back();
		m_events.pop_back();
		return true;
	}
private:
	std::deque<EventId> m_events;
};

