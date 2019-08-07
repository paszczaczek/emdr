#pragma once
#include "Debug.h"

template<class TEventArgs>
class EventHandlerBase
{
public:
	virtual void Execute(TEventArgs& eventArgs) = 0;
  virtual ~EventHandlerBase() {}
};


template<class TTarget, class TEventArgs>
class EventHandler : public EventHandlerBase<TEventArgs>
{
private:
	typedef void(TTarget::*Function)(TEventArgs&);

public:
  /*
	EventHandler(TTarget* target, Function callback)
	{
		this->target = target;
		this->function = callback;
	}
  */
  EventHandler& Set(TTarget* target, Function callback)
  {
    this->target = target;
    this->function = callback;
    return *this;
  }

private:
	void Execute(TEventArgs& eventArgs) override
	{
    Serial.println("    EventHandler begin");
    //PRINT_FREEMEM("   Execute begin");
		(target->*function)(eventArgs);
    Serial.println("    EventHandler end");
	}

	TTarget* target;
	Function function;
};
