#pragma once
//#include "EventName.h"

class Event
{
public:
	enum struct Name
	{
		UnknowCode = 0,

		BlockingInterruptsDisallowed = 1,
		BlockingInterruptsAllowed = 2,

		Start = 3,
		Stop = 4,
		Pause = 5,

		SELECT_STRIP_DEVICE,
		SELECT_LASER_DEVICE,
		
		CHANEL_PLUS,
		CHANEL_MINUS
	};

	static void Send(Name name) {};
};