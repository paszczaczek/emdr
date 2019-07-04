#pragma once
#include <Arduino.h>
#include "Event.h"

class TimerEventArgs
{
public:
	TimerEventArgs(unsigned int elapsedIntervals)
	{
		this->elapsedIntervals = elapsedIntervals;
	}
	unsigned int elapsedIntervals = 0;
};

class Timer
{
public:
	unsigned long interval = 0;
	bool autoReset = true;
	Event<TimerEventArgs> elapsed;

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
			elapsed.Emit(TimerEventArgs(elapsedIntervals));
		}
	}

private:
	unsigned long startedAt = 0;
	unsigned long elapsedAt = 0;
};