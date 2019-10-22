#pragma once
//#include "EventName.h"

class Event
{
public:
	enum class Name
	{
		UnknowCode = 0,

		BlockingInterruptsDisallowed = 1,
		BlockingInterruptsAllowed = 2,

		SELECT_STRIP_DEVICE,
		SELECT_LASER_DEVICE,

		CHANEL_PLUS,
		CHANEL_MINUS
	};

	static void Send(Name name);
};