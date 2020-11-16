#pragma once

class Event
{
public:
	// enum Name jest wspoluzytkowany przez projekt EmdrEmulator (c#)
#include "EventName.h"

	static void Send(Name name);
};