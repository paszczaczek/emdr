#pragma once
#include <Arduino.h>
#include <stdio.h>
#include "Event.h"

class TimerOLD
{
public:
	class EventArgs
	{
	public:
		unsigned int elapsedIntervals;
		unsigned int elapsedIntervalsFromStart;
	};

	TimerOLD(byte handlersCapacity = 1) : elapsed(handlersCapacity) { }

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
	// czas uruchomienia timera w milisekundach
	unsigned long startedAt = (unsigned long)-1;

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
	bool IsStarted()
	{
		return startedAt != (unsigned long)-1;
	}

	// czy nadszedl czas aktywacji timera
	bool Elapsed(unsigned int* ommittedIntervals = NULL)
	{
		// czy timer zostal uruchomiony?
		if (!IsStarted())
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
		unsigned int elapsedIntervals = (now - startedAt) / interval;

		// liczba pominietych intervalow
		if (ommittedIntervals)
		{
			if (elapsedIntervals > 0)
				* ommittedIntervals = elapsedIntervals - 1;
			else
				*ommittedIntervals = 0;
		}

		// czy nadszel aktywacji
		return elapsedIntervals > 0;
	}
};


// Timer aktywowany jednokrotnie
class Timer : public TimerBase
{
public:
	// czy nadszedl czas aktywacji timera
	bool Elapsed(unsigned int* ommittedIntervals = NULL)
	{
		bool elapsed = TimerBase::Elapsed(ommittedIntervals);
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
	bool Elapsed(unsigned int* outOmmittedIntervals)
	{
		unsigned int ommittedIntervals;
		bool elapsed = TimerBase::Elapsed(&ommittedIntervals);
		// jesli nadszedl czas aktywacji, to aktywujemy nastepny cykl
		if (elapsed)
			startedAt += (1 + ommittedIntervals) * interval;
		if (outOmmittedIntervals)
			* outOmmittedIntervals = ommittedIntervals;
		return elapsed;
	}
};


// Licznik odliczajacy od 1 do nieskonczonosci
class CounterInfinite : public TimerRepeatable
{
protected:
	// ile minelo interwalow od czasu wystartowania licznika
	unsigned int counter = (unsigned int)-1;

public:
	// wystartowanie licznika
	void Start()
	{
		TimerRepeatable::Start();
		//elaspsedIntervals = 0;
	}

	// zatrzymanie licznika
	void Stop()
	{
		TimerRepeatable::Stop();
		counter = (unsigned int)-1;
	}

	// czy nadszedl czas aktywacji licznika
	bool Elapsed(unsigned int& outCounter, unsigned int* outOmmittedCounters = NULL)
	{
		unsigned int ommitedCounters;
		bool elapsed = TimerRepeatable::Elapsed(&ommitedCounters);
		if (elapsed)
			counter += 1 + ommitedCounters;
		outCounter = counter;
		if (outOmmittedCounters)
			* outOmmittedCounters = ommitedCounters;
		return elapsed;
	}
};


// Licznik odliczajacy w gore od 1 do zadanej wartosci
class Counter : public CounterInfinite
{
public:
	// wartosc do ktorej odliczac
	unsigned int countTo = 0;

public:
	// czy nadszedl czas aktywacji licznika
	bool Elapsed(unsigned int& counter)
	{
		// counterTo musi byc podany
		if (countTo == 0 && IsStarted())
		{
			Serial.println(F("* Counter:counterTo=0!"));
			Stop();
			return false;
		}

		bool elapsed = CounterInfinite::Elapsed(counter);
		if (elapsed && counter >= countTo)
			Stop();
		return elapsed;
	}
};


// Licznik odliczajacy w gore od 1 do zadanej wartosci i w dol
class CounterUpDownRepeadable : public CounterInfinite
{
public:
	// wartosc do ktorej odliczac
	unsigned int countTo = 0;
	enum struct Info : byte { MinOccurred, MaxOccurred, MinOmmited, MaxOmmited, None };

public:
	void Pause()
	{
		TimerRepeatable::Stop();
	}

	void Resume()
	{
		TimerRepeatable::Start();
	}

	// czy nadszedl czas aktywacji countera
	bool Elapsed(unsigned int& counter, Info* info = NULL, unsigned int* ommittedMinMaxCounters = NULL)
	{
		// inicjalizacja parametrow wyjsciowych
		counter = 0;
		if (info)
			* info = Info::None;
		if (ommittedMinMaxCounters)
			ommittedMinMaxCounters = 0;

		// od razu po uruchomieniu licznika jest pierwsza aktywacja z counterem zerowycm
		if (this->counter == (unsigned int)-1)
		{
			this->counter = 0;
			return true;
		}

		// counterTo musi byc podany
		if (countTo <= 1 && IsStarted())
		{
			Serial.println(F("* CounterUpDownRepeadable::counterTo=0!"));
			Stop();
			return false;
		}

		// czy naszedla czas aktywacji licznika?
		unsigned int ommittedCounters;
		bool elapsed = CounterInfinite::Elapsed(counter, &ommittedCounters);
		if (elapsed)
		{
			// tak, nadszedl czas aktywacji
			counter = this->counter % countTo;
			bool fallingSlope = this->counter / countTo % 2 == 1;
			if (fallingSlope)
				if (counter == 0)
					counter = countTo;
				else
					counter = countTo - counter;

			if (counter == 0)
			{
				if (info)
					* info = Info::MinOccurred;
			}
			else if (counter == countTo) {
				if (info)
					* info = Info::MaxOccurred;
			}
			else if (fallingSlope)
			{
				if (ommittedCounters >= countTo - counter)
				{
					if (info)
						* info = Info::MaxOmmited;
					if (ommittedMinMaxCounters)
						* ommittedMinMaxCounters = countTo - counter;
				}
			}
			else // raisingSlope
			{
				if (ommittedCounters >= counter)
				{
					if (info)
						* info = Info::MinOmmited;
					if (ommittedMinMaxCounters)
						* ommittedMinMaxCounters = counter;
				}
			}

			Debug(this->counter, counter, ommittedCounters, fallingSlope, info);
		}
		return elapsed;
	}

	void Decrease(int delta)
	{
		counter -= delta;
	}

private:
	void Debug(unsigned int counter, unsigned int counterModulo, unsigned int ommittedCounters, bool fallingSlope, Info* info)
	{
		char buf[255] = "";
		char slope = fallingSlope ? '\\' : '/';
		if (counterModulo == 0)
			slope = 'v';
		else if (counterModulo == countTo)
			slope = '^';
		snprintf(buf, sizeof buf, "%ud: %c%d !%d (%d)",
			counter, slope, counterModulo, ommittedCounters, info ? (int)* info : -1);
		Serial.println(buf);
	}
};


class CounterUpDownRepeadable2 : public CounterUpDownRepeadable
{
public:
	// wartosc do ktorej odliczac
	unsigned int countTo = 0;
	enum struct Info : byte { MinOccurred, MaxOccurred, MinOmmited, MaxOmmited, None };

public:
	void Pause()
	{
		TimerRepeatable::Stop();
	}

	void Resume()
	{
		TimerRepeatable::Start();
	}

	// czy nadszedl czas aktywacji countera
	bool Elapsed(unsigned int& counter, Info* info = NULL, unsigned int* ommittedMinMaxCounters = NULL)
	{
		// inicjalizacja parametrow wyjsciowych
		counter = 0;
		if (info)
			* info = Info::None;
		if (ommittedMinMaxCounters)
			ommittedMinMaxCounters = 0;

		// od razu po uruchomieniu licznika jest pierwsza aktywacja z counterem zerowycm
		if (this->counter == (unsigned int)-1)
		{
			this->counter = 0;
			return true;
		}

		// counterTo musi byc podany
		if (countTo <= 1 && IsStarted())
		{
			Serial.println(F("* CounterUpDownRepeadable::counterTo=0!"));
			Stop();
			return false;
		}

		// czy naszedla czas aktywacji licznika?
		unsigned int ommittedCounters;
		bool elapsed = CounterInfinite::Elapsed(counter, &ommittedCounters);
		if (elapsed)
		{
			// tak, nadszedl czas aktywacji
			counter = this->counter % countTo;
			bool fallingSlope = this->counter / countTo % 2 == 1;
			if (fallingSlope)
				if (counter == 0)
					counter = countTo;
				else
					counter = countTo - counter;

			if (counter == 0)
			{
				if (info)
					* info = Info::MinOccurred;
			}
			else if (counter == countTo) {
				if (info)
					* info = Info::MaxOccurred;
			}
			else if (fallingSlope)
			{
				if (ommittedCounters >= countTo - counter)
				{
					if (info)
						* info = Info::MaxOmmited;
					if (ommittedMinMaxCounters)
						* ommittedMinMaxCounters = countTo - counter;
				}
			}
			else // raisingSlope
			{
				if (ommittedCounters >= counter)
				{
					if (info)
						* info = Info::MinOmmited;
					if (ommittedMinMaxCounters)
						* ommittedMinMaxCounters = counter;
				}
			}

			Debug(this->counter, counter, ommittedCounters, fallingSlope, info);
		}
		return elapsed;
	}

	void Decrease(int delta)
	{
		counter -= delta;
	}

private:
	void Debug(unsigned int counter, unsigned int counterModulo, unsigned int ommittedCounters, bool fallingSlope, Info* info)
	{
		char buf[255] = "";
		char slope = fallingSlope ? '\\' : '/';
		if (counterModulo == 0)
			slope = 'v';
		else if (counterModulo == countTo)
			slope = '^';
		snprintf(buf, sizeof buf, "%ud: %c%d !%d (%d)",
			counter, slope, counterModulo, ommittedCounters, info ? (int)* info : -1);
		Serial.println(buf);
	}
};