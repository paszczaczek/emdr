#pragma once
//#include "EventName.h"
#include "Event.h"

class Device
{
public:
	virtual bool ReceiveEvent(Event::Name eventName) { return false; }
};

extern Device& stripDevice;