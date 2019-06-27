#pragma once
#include <vector>
#include "EventHandler.h"

template <class TEventArgs>
class Event
{

public:
	Event operator+= (EventHandlerBase<TEventArgs>& eventHandler)
	{
		eventHandlers.push_back(&eventHandler);
		return *this;
	}

	void Emit(TEventArgs& eventArgs)
	{
		for (auto iter = eventHandlers.begin(); iter < eventHandlers.end(); iter++)
			(*iter)->Execute(eventArgs);
	}

private:
	std::vector<EventHandlerBase<TEventArgs>*> eventHandlers;
};