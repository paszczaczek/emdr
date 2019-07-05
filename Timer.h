#pragma once
#include <Arduino.h>
#include "Event.h"

class Timer
{
public:
	class EventArgs
	{
	public:
		EventArgs(unsigned int elapsedIntervals)
		{
			this->elapsedIntervals = elapsedIntervals;
		}
		unsigned int elapsedIntervals = 0;
	};

	unsigned long interval = 0;
	bool autoReset = true;
	Event<EventArgs> elapsed;

	void Start()
	{
		elapsedAt = startedAt = millis();
	}

	void Loop() 
	{
		auto now = millis();
		unsigned int elapsedIntervals = (now - elapsedAt) / interval;
		if (elapsedIntervals != 0)
		{
			elapsedAt += elapsedIntervals * interval;
			elapsed.Emit(EventArgs(elapsedIntervals));
		}
	}

private:
	unsigned long startedAt = 0;
	unsigned long elapsedAt = 0;
};