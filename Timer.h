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
class TimerPeriodic : public TimerBase
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


// Licznik odliczajacy od 0 do nieskonczonosci
class CounterInfinite : public TimerPeriodic
{
protected:
	// ile minelo interwalow od czasu wystartowania licznika
	unsigned int counter = (unsigned int)-1;

public:
	// wystartowanie licznika
	void Start()
	{
		TimerPeriodic::Start();
	}

	// zatrzymanie licznika
	void Stop()
	{
		TimerPeriodic::Stop();
		counter = (unsigned int)-1;
	}

	// zapauzowanie licznika
	void Pause()
	{
		TimerPeriodic::Stop();
	}

	// wznowienie licznika
	void Resume()
	{
		TimerPeriodic::Start();
	}

	// czy nadszedl czas aktywacji licznika
	bool Elapsed(unsigned int& outCounter, unsigned int* outOmmittedCounters = NULL)
	{
		// inicjalizacja wartosci zwrotnych
		outCounter = counter;
		if (outOmmittedCounters)
			* outOmmittedCounters = 0;

		// od razu po uruchomieniu licznika jest pierwsza aktywacja z counterem zerowycm
		if (counter == (unsigned int)-1)
		{
			counter = outCounter = 0;
			return true;
		}

		// czy nadszedl czas aktywacji licznika?
		unsigned int ommitedCounters;
		bool elapsed = TimerPeriodic::Elapsed(&ommitedCounters);
		if (elapsed)
		{
			counter += 1 + ommitedCounters;
			outCounter = counter;
			if (outOmmittedCounters)
				* outOmmittedCounters = ommitedCounters;
		}
		return elapsed;
	}
};


// Licznik odliczajacy okresowo od 0 do zadanej wartosci
class CounterPeriodic : public CounterInfinite
{
public:
	// wartosc do ktorej odliczac
	unsigned int countTo = 0;

public:

	// czy nadszedl czas aktywacji countera
	bool Elapsed(unsigned int& counter, unsigned int* period = NULL, unsigned int* outOmmittedCounters = NULL)
	{
		// inicjalizacja wartosci zwrotnych
		counter = this->counter;
		if (period)
			* period = 0;
		if (outOmmittedCounters)
			* outOmmittedCounters = 0;

		// counterTo musi byc podany
		if (countTo <= 1 && IsStarted())
		{
			Serial.println(F("* CounterPeriodic::counterTo=0!"));
			Stop();
			return false;
		}

		// czy naszedla czas aktywacji licznika?
		unsigned int ommittedCounters;
		bool elapsed = CounterInfinite::Elapsed(counter, &ommittedCounters);
		if (elapsed)
		{
			// tak, nadszedl czas aktywacji
			// na podstawie licznika liczacego do nieskonczonosci wyznaczamy wartosc licznika okresowego
			counter = this->counter % (countTo + 1);
		}

		// wartosci zwrotne
		if (period)
			* period = this->counter / (countTo + 1);
		if (outOmmittedCounters)
			* outOmmittedCounters = ommittedCounters;

		return elapsed;
	}
};


// Licznik odliczajacy od 0 do zadanej wartosci
//class Counter : public CounterInfinite
//{
//public:
//	// wartosc do ktorej odliczac
//	unsigned int countTo = 0;
//
//public:
//	// czy nadszedl czas aktywacji licznika
//	bool Elapsed(unsigned int& counter)
//	{
//		// counterTo musi byc podany
//		if (countTo == 0 && IsStarted())
//		{
//			Serial.println(F("* Counter:counterTo=0!"));
//			Stop();
//			return false;
//		}
//
//		bool elapsed = CounterInfinite::Elapsed(counter);
//		if (elapsed && counter >= countTo)
//			Stop();
//		return elapsed;
//	}
//};


// Licznik odliczajacy w gore i w dol od 0 do zadanej wartosci
class CounterUpDownPeriodic : public CounterInfinite
{
public:
	// wartosc do ktorej odliczac
	unsigned int countTo = 0;

public:

	// czy nadszedl czas aktywacji countera
	bool Elapsed(unsigned int& outCounter, unsigned int* outPeriod, unsigned int* outOmmittedCounters = NULL)
	{
		// inicjalizacja wartosci zwrotnych
		outCounter = 0;
		if (outPeriod)
			* outPeriod = 0;
		if (outOmmittedCounters)
			* outOmmittedCounters = 0;

		// counterTo musi byc podany
		if (countTo <= 1 && IsStarted())
		{
			Serial.println(F("* CounterUpDownPeriodic::counterTo=0!"));
			Stop();
			return false;
		}

		// czy naszedla czas aktywacji licznika?
		unsigned int ommittedCounters;
		bool elapsed = CounterInfinite::Elapsed(counter, &ommittedCounters);
		if (elapsed)
		{
			// tak, nadszedl czas aktywacji
			/*
				countTo = 4
						x
					  x   x
					x       x   x
				  x           x
				0 1 2 3 4 5 6 7 8  counter
			   -1 0 1 2 3 4 5 6 7  counter - 1
				  0 1 2 0 1 2 0 1  (counter - 1) % (countTo - 1)
				  0 0 0 1 1 1 2 2  (counter - 1) / (countTo - 1)
						t t t      fallingSlope
			*/
			// counter == 0 pojawia sie zaraz po uruchomieniu licznika
			if (counter > 0)
			{
				// counter to licznik odliczajacy od 1 do nieskonczonosci
				// outCounter to licznik odliczajacy od 1 do countTo
				// obliczenia modulo trzeba robic na liczbach od zera, a counter jest od 1
				// dlatego przed operacja modulo trzeba od countera odjac 1 i do wyniku dodac 1
				outCounter = (counter - 1) % (countTo - 1);
				bool fallingSlope = (counter - 1) / (countTo - 1) % 2 == 1;
				if (fallingSlope)
					if (outCounter == 0)
						outCounter = countTo;
					else
						outCounter = countTo - outCounter;
				else
					outCounter = outCounter + 1;

				// wartosci zwrotne
				if (outPeriod)
					* outPeriod = (counter - 1) / (2 * countTo - 2);
				if (outOmmittedCounters)
					* outOmmittedCounters = ommittedCounters;
			}

			//fprintf(stderr, "%d: %d (%d)\n", counter, outCounter, *outPeriod);
		}

		return elapsed;
	}
};

// Licznik odliczajacy w gore i w dol od 0 do zadanej wartosci z gwarancja trafienia w 0 i max
class CounterUpDownPeriodicDetectingMinMax : public CounterUpDownPeriodic
{
public:
	// czy nadszedl czas aktywacji licznika
	bool Elapsed(unsigned int& counter, unsigned int* period = NULL)
	{
		// czy naszedla czas aktywacji licznika?
		unsigned int ommittedCounters;
		bool elapsed = CounterUpDownPeriodic::Elapsed(counter, period, &ommittedCounters);
		if (elapsed)
		{
			bool fallingSlope = this->counter / countTo % 2 == 1;
			if (counter != 0 && counter != countTo)
				if (fallingSlope)
				{
					if (ommittedCounters >= countTo - counter)
					{
						this->counter -= countTo - counter;
						counter = countTo;
					}
				}
				else // raisingSlope
				{
					if (ommittedCounters >= counter)
					{
						this->counter -= counter;
						counter = 0;
					}
				}
			//Debug(this->counter, counter, ommittedCounters, fallingSlope, info);
		}
		return elapsed;
	}

	//private:
	//	void Debug(unsigned int counter, unsigned int counterModulo, unsigned int ommittedCounters, bool fallingSlope, Info* info)
	//	{
	//		char buf[255] = "";
	//		char slope = fallingSlope ? '\\' : '/';
	//		if (counterModulo == 0)
	//			slope = 'v';
	//		else if (counterModulo == countTo)
	//			slope = '^';
	//		snprintf(buf, sizeof buf, "%ud: %c%d !%d (%d)",
	//			counter, slope, counterModulo, ommittedCounters, info ? (int)* info : -1);
	//		Serial.println(buf);
	//	}
};