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


// Klasa bazowa dla Timerow i Counterow, wylicza czasy aktywacji
class TimerBase
{
public:
	// po ilu milisekundach aktywowac timer
	unsigned long interval = 0;

protected:
	// czas uruchomienia timera w milisekundach od startu systemu
	unsigned long startedAt = -1;

protected:
	// konstruktor protected nie pozwala tworzyc instacji tej klasy
	TimerBase() {}

public:
	// wystartowanie timera
	void Start()
	{
		startedAt = millis();
	}

	// zatrzymanie timera
	void Stop()
	{
		startedAt = (unsigned long)-1;
	}

	// czy timer jest wystartowany
	bool Started()
	{
		return startedAt != (unsigned long)-1;
	}

	// czy nadszedl czas aktywacji timera
	bool Elapsed(unsigned int& elapsedIntervals)
	{
		// czy timer zostal uruchomiony?
		if (!Started())
			return false;

		// interval musi byc podany
		if (interval == 0)
		{
			Serial.println(F("* Timer::interval=0!"));
			startedAt = -1;
			return false;
		}

		// ile minelo pelnych intervalow od momentu startu
		auto now = millis();
		elapsedIntervals = (now - startedAt) / interval;

		// czy nadszel aktywacji
		return elapsedIntervals > 0;
	}
};


// Timer aktywowany jednokrotnie
class Timer2 : public TimerBase
{
public:
	// czy nadszedl czas aktywacji timera
	bool Elapsed(unsigned int& elapsedIntervals)
	{
		bool elapsed = TimerBase::Elapsed(elapsedIntervals);
		// jesli nadszedl czas aktywacji, to stopujemy timer
		if (elapsed)
			Stop();
		return elapsed;
	}
};


// Timer aktywowany wielokorotnie
class TimerRepeatable : public TimerBase
{
public:
	// czy nadszedl czas aktywacji timera
	bool Elapsed(unsigned int& elapsedIntervals)
	{
		bool elapsed = TimerBase::Elapsed(elapsedIntervals);
		// jesli nadszedl czas aktywacji, to aktywujemy nastepny cykl
		if (elapsed)
			startedAt += elapsedIntervals * interval;
		return elapsed;
	}
};


// Licznik odliczajacy od 1 do nieskonczonosci
class CounterUp : public TimerRepeatable
{
private:
	// ile minelo interwalow od czasu wystartowania licznika
	unsigned int elaspsedIntervalsFromStart = 0;

public:
	// wystartowanie licznika
	void Start()
	{
		TimerRepeatable::Start();
		elaspsedIntervalsFromStart = 0;
	}

	// czy nadszedl czas aktywacji licznika
	bool Elapsed(unsigned int& counter)
	{
		unsigned int elapsedIntervals = 0;
		bool elapsed = TimerRepeatable::Elapsed(elapsedIntervals);
		if (elapsed)
		{
			elaspsedIntervalsFromStart += elapsedIntervals;
			counter = elaspsedIntervalsFromStart;
		}
		return elapsed;
	}
};


// Licznik odliczajacy w gore od 1 do zadanej wartosci
class CounterUpTo : public CounterUp
{
public:
	// wartosc do ktorej odliczac
	unsigned int countTo = 0;

public:
	// czy nadszedl czas aktywacji licznika
	bool Elapsed(unsigned int& counter)
	{
		// counterTo musi byc podany
		if (countTo == 0 && Started())
		{
			Serial.println(F("* CounterUpTo::counterTo=0!"));
			Stop();
			return false;
		}

		bool elapsed = CounterUp::Elapsed(counter);
		if (elapsed && counter >= countTo)
			Stop();
		return elapsed;
	}
};

#include <stdio.h>
// Licznik odliczajacy w gore od 1 do zadanej wartosci i w dol
class CounterUpToDown : public CounterUp
{
public:
	// wartosc do ktorej odliczac
	unsigned int countTo = 0;

public:
	// czy nadszedl czas aktywacji countera
	bool Elapsed(unsigned int& counter)
	{
		// counterTo musi byc podany
		if (countTo <= 1 && Started())
		{
			Serial.println(F("* CounterUpToDown::counterTo=0!"));
			Stop();
			return false;
		}

		bool elapsed = CounterUp::Elapsed(counter);
		if (elapsed)
		{
			/*
			countTo = 5
			|1| 2 3 4 |5| 6 7 8 |9| 10 - counter
			|0| 1 2 3 |4| 5 6 7 |8|  9 - counter - 1
			|0| 1 2 3 |0| 1 2 3 |0|  1 - (counter - 1) % (countTo - 1)
			|1| 2 3 4 |1| 1 2 3 |1|  5 - (counter - 1) % (countTo - 1) + 1
			*/
			char buf[255] = "";
			bool boundry = (counter - 1) % (countTo - 1) == 0;
			bool up = (counter - 1) / (countTo - 1) % 2 == 0;
			int cnt = 0;
			if (boundry)
				if (up)
					cnt = 1;
				else
					cnt = countTo;
			else
				if (up)
					cnt = (counter - 1) % (countTo - 1) + 1;
				else
					cnt = countTo - (counter - 1) % (countTo - 1);
			sprintf(buf, "%d %d %s : %d", counter, boundry, up ? "UP" : "dn", cnt);
			Serial.println(buf);
			/*
			bool countUp = (counter - 1) / (countTo -1 ) % 2 == 0;
			counter = (counter - 1) % countTo + 1;
			if (countUp)
				;
			else
				counter = countTo - counter;
				*/
				/*
				bool countUp = counter / countTo % 2 == 0 || counter % countUp == 0;
				if (countUp)
				{
					counter %= countTo + 1;
				}
				else
				{
					counter  = countTo - counter % countTo;
				}
				*/
		}
		return elapsed;
	}
};