#pragma once
#include "EmdrController/Event.h"

class Device
{
public:
	virtual void Loop() = 0;
	virtual bool Receive(Event::Name eventName) = 0;
};

extern Device& stripDevice;