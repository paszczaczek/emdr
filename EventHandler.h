#pragma once
#include "EventArgs.h"

template<class TEventArgs>
class EventHandlerBase
{
public:
	virtual void Execute(TEventArgs& eventArgs) = 0;
};


template<class TTarget, class TEventArgs>
class EventHandler : public EventHandlerBase<TEventArgs>
{
private:
	typedef void(TTarget::*Function)(TEventArgs&);

public:
	EventHandler(TTarget* target, Function callback)
	{
		this->target = target;
		this->function = callback;
	}

private:
	void Execute(TEventArgs& eventArgs) override
	{
		(target->*function)(eventArgs);
	}

	TTarget* target;
	Function function;
};