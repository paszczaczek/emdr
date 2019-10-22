#pragma once

class Event
{
public:
	enum struct Type
	{
		SELECT_STRIP_DEVICE,
		SELECT_LASER_DEVICE
	};

	static void Send(Type event) {}
};