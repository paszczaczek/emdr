#pragma once
#include "EventArgs.h"

template<class TEventArgs>
class EventHandlerBase
{
public:
	virtual void Execute(TEventArgs& eventArgs) = 0;
};


template<class TEventArgs, class TTarget>
class EventHandler : public EventHandlerBase<TEventArgs>
{
private:
	typedef void(TTarget::*Callback)(TEventArgs&);

public:
	EventHandler(TTarget* target, Callback callback)
	{
		this->target = target;
		this->callback = callback;
	}

private:
	void Execute(TEventArgs& eventArgs) override
	{
		(target->*callback)(eventArgs);
	}

	TTarget* target;
	Callback callback;
};