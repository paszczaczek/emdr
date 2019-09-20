#pragma once
#include <Arduino.h>
#include "Event.h"

class Timer
{
public:
	class EventArgs
	{
	public:
		unsigned int elapsedIntervals;
		unsigned int elapsedIntervalsFromStart;
	};

	Timer(byte handlersCapacity = 1) : elapsed(handlersCapacity) { }

	unsigned long interval = 0;
	bool autoReset = true;
	Event<EventArgs> elapsed;

	void Start()
	{
		elapsedAt = startedAt = millis();
	}

	void Stop()
	{
		startedAt = elapsedAt = -1;
	}

	void Loop()
	{
		if (startedAt == (unsigned long)-1)
			return;
		auto now = millis();
		if (interval == 0)
		{
			Serial.println("Timer::Loop /0 !");
			Stop();
		}
		unsigned int elapsedIntervals = (now - elapsedAt) / interval;
		if (elapsedIntervals != 0)
		{
			elapsedAt += elapsedIntervals * interval;
			args.elapsedIntervals = elapsedIntervals;
			args.elapsedIntervalsFromStart = (elapsedAt - startedAt) / interval;
			elapsed.Emit(args);
			if (!autoReset)
				Stop();
		}
	}

private:
	unsigned long startedAt = -1;
	unsigned long elapsedAt = -1;
	EventArgs args;
};

class Timer2
{
public:
	unsigned long interval = 0;
	bool autoReset = true;

private:
	unsigned long startedAt = -1;

public:
	void Start()
	{
		startedAt = millis();
	}

	void Stop()
	{
		startedAt = (unsigned long)-1;
	}

	bool Elapsed(unsigned int& elapsedIntervals)
	{
		// czy timer zostal wystartowany?
		if (startedAt == (unsigned long)-1)
			return false;

		// interval musi byc podany
		if (interval == 0)
		{
			Serial.println(F("Timer::interval=0 !"));
			Stop();
			return false;
		}

		// ile minelo pelnych intervalow od momentu startu
		auto now = millis();
		elapsedIntervals = (now - startedAt) / interval;

		// czy nadszel aktywacji
		if (elapsedIntervals == 0)
		{
			// nie, jeszcze nie nadszedl czas aktywacji
			return false;
		}
		else
		{
			// tak, nadszedl czas aktywacji
			// czy timer ma wystartowac ponownie?
			if (autoReset)
			{

				// tak - wyznaczamy nowy czas startu
				startedAt += elapsedIntervals * interval;
			}
			else
			{
				// nie - stopujemy timer
				Stop();
			}
			return true;
		}
	}
};
