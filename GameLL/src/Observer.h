#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include "EntityMessages.h"
#include "C_State.h"
using MessageType = unsigned int;
struct TwoFloats { float x; float y; };
class Observer;
using ObserverContainer = std::vector<Observer*>;
struct Message {
	Message(const MessageType& i_type) :m_type(i_type) {}
	MessageType m_type;
	int m_sender;
	int m_receiver;
	//used for s_state and s_sound to stop the sounds
	int m_oldState;
	union {
		TwoFloats m_2F;
		bool m_bool;
		int m_int;
	};
};
class Observer
{
public:
	virtual ~Observer();
	virtual void Notify(const Message& i_message) = 0;
};

class Communicator {
public:
	~Communicator() { m_observers.clear(); };
	bool AddObserver(Observer* i_observer){
		if (HasObserver(i_observer)) { return false; }
		m_observers.emplace_back(i_observer);
		return true;
	}
	bool RemoveObserver(Observer* i_observer) {
		if (!HasObserver(i_observer)) { return false; }
		auto observer = std::find_if(m_observers.begin(), m_observers.end(), [&i_observer](Observer* o) {
			return o == i_observer;
			});
		if (observer == m_observers.end()) { return false; }
		m_observers.erase(observer);
		return true;
	}
	bool HasObserver(const Observer* i_observer) {
		return std::find_if(m_observers.begin(), m_observers.end(), [&i_observer](Observer* o) {
			return o == i_observer;
			}) != m_observers.end();
	}
	void BroadCast(const Message& i_msg) {
		
		for (auto& itr : m_observers) { 
			itr->Notify(i_msg); }
	}

private:
	ObserverContainer m_observers;
};

using Subscriptions = std::unordered_map<EntityMessage, Communicator>;
class MessageHandler {
public:
	bool Subscribe(const EntityMessage& i_type, Observer* i_observer) {
		return m_communicators[i_type].AddObserver(i_observer);
	}
	bool Unsubscribe(const EntityMessage& i_type, Observer* i_observer) {
		return m_communicators[i_type].RemoveObserver(i_observer);
	}
	void Dispatch(const Message& i_msg) {
		auto itr = m_communicators.find((EntityMessage)i_msg.m_type);
		if (itr == m_communicators.end()) { return; }
		itr->second.BroadCast(i_msg);
	}
	void QueueMessage(const Message& i_msg) {
		m_queuedMessages.push(i_msg);
	}
	void ProcessQueuedMessages() {
		while (!m_queuedMessages.empty())
		{
			Dispatch(m_queuedMessages.front());
			m_queuedMessages.pop();
		}
	}
private:
	Subscriptions m_communicators;
	std::queue<Message> m_queuedMessages;
};