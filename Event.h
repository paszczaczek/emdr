#pragma once
//#include "EventName.h"

class Event
{
public:
	enum struct Name
	{
		SELECT_STRIP_DEVICE,
		SELECT_LASER_DEVICE
	};

	static void Send(Name name);
};