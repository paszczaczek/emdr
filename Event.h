#pragma once
#include <ArduinoSTL.h>
#include <vector>
#include "EventHandler.h"

template <class TEventArgs>
class Event
{
public:
	~Event()
	{
		for (auto eh : _eventHandlers)
			delete eh;
	}

	Event& operator += (EventHandlerBase<TEventArgs> *eventHandler)
	{
		_eventHandlers.push_back(eventHandler);

		return *this;
	}
	
	Event& operator -= (EventHandlerBase<TEventArgs> *eventHandler)
	{
		auto it = std::find(_eventHandlers.begin(), _eventHandlers.end(), eventHandler);
		if (it != _eventHandlers.end())
		{
			delete *it;
			_eventHandlers.erase(it);
		}

		return *this;
	}

	void Emit(TEventArgs&& eventArgs)
	{
		//for (auto it = _eventHandlers.begin(); it < _eventHandlers.end(); it++)
		//	(*it)->Execute(eventArgs);
		for (auto eh : _eventHandlers)
			eh->Execute(eventArgs);
	}

private:
	std::vector<EventHandlerBase<TEventArgs>*> _eventHandlers;
};
