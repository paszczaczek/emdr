#pragma once
#include "EmdrController/Event.h"

class Device
{
public:
	virtual void Loop() = 0;
	virtual bool Receive(Event::Name eventReceived) = 0;
};

extern Device& stripDevice;