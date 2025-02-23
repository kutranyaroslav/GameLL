#pragma once
#include <queue>
using EventId = unsigned int;

class EventQueue
{
public:
	void AddEvent(const EventId& i_event) { m_events.push(i_event); }
	void Clear() { while (!m_events.empty()) { m_events.pop();  } }
	bool ProcessEvents(EventId& i_id) {
		if (m_events.empty()) { return false; }
		i_id = m_events.front();
		m_events.pop();
		return true;
	}
private:
	std::queue<EventId> m_events;
};

