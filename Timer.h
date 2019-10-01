#pragma once
#include <Arduino.h>
#include <stdio.h>
#include "Event.h"


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
				*ommittedIntervals = elapsedIntervals - 1;
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
			*outOmmittedIntervals = ommittedIntervals;
		return elapsed;
	}
};


// Licznik w roznych warianach
class Counter : public TimerPeriodic
{
public:
	// warianty licznika
	enum Mode : byte { Up, Down };

	// opcje licznika
	enum Options : byte { None, WithZero };

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
	bool ItsTime(Options options, unsigned int& outCounter, unsigned int* outOmmittedCounters = NULL)
	{
		// inicjalizacja wartosci zwrotnych
		outCounter = counter;
		if (outOmmittedCounters)
			*outOmmittedCounters = 0;

		// counter == -1 oznacza ze timer dopiero co zostal uruchomiony
		if (counter == (unsigned int)-1)
		{
			counter = outCounter = 0;
			// na zyczenie aktywacja licznika z counterem zerowycm 
			return options & Options::WithZero;
		}

		// czy nadszedl czas aktywacji licznika?
		unsigned int ommitedCounters;
		bool elapsed = TimerPeriodic::Elapsed(&ommitedCounters);
		if (elapsed)
		{
			counter += 1 + ommitedCounters;
			outCounter = counter;
			if (outOmmittedCounters)
				*outOmmittedCounters = ommitedCounters;
		}
		return elapsed;
	}
};



// Licznik okresowy w roznych wariantach
class CounterPeriodic : public Counter
{
public:
	// wartosc do ktorej odliczac
	unsigned int countTo = 0;

	// warianty licznika
	enum Mode : byte { Up, Down, UpDown, DownUp };

	// opcje licznika
	enum Options : byte { None, WithZero, CatchMinMax };

public:
	bool ItsTime(Mode mode, Options options, unsigned int& outCounter, unsigned int* outPeriod, unsigned int* outOmmittedCounters = NULL)
	{
		// inicjalizacja wartosci zwrotnych
		outCounter = 0;
		if (outPeriod)
			*outPeriod = 0;
		if (outOmmittedCounters)
			*outOmmittedCounters = 0;

		// czy licznik jest wystartowany?
		if (!IsStarted())
		{
			// nie
			return false;
		}
		else
		{
			// licznik wystartowany - counterTo musi byc podany
			if (countTo <= 1)
			{
				Serial.println(F("* CounterPeriodic::counterTo=0!"));
				Stop();
				return false;
			}
		}

		// czy naszedla czas aktywacji licznika?
		unsigned int ommittedCounters;
		bool itsTime = Counter::ItsTime(
			options & CounterPeriodic::Options::WithZero ? Counter::Options::WithZero : Counter::Options::None,
			counter,
			&ommittedCounters);

		// jesli nie to konczymy
		if (!itsTime || counter == 0)
			return itsTime;

		// nadszedl czas aktywacji licznika, wyznaczamy wartosci licznika i okresu
		const char* notImplementedYet = F("* Not implemented yet!");
		switch (mode)
		{
		case CounterPeriodic::Up:     Serial.println(notImplementedYet); Stop();  return false;
		case CounterPeriodic::Down:   Serial.println(notImplementedYet); Stop();  return false;
		case CounterPeriodic::UpDown: CalculationForUpDownCounter(outCounter, outPeriod); break;
		case CounterPeriodic::DownUp: Serial.println(notImplementedYet); Stop();  return false;
		}

		// wylapanie ominietego 1 lub max
		if (options & Options::CatchMinMax)
			CatchMinMaxUpDown(outCounter, outPeriod, ommittedCounters);

		if (outOmmittedCounters)
			*outOmmittedCounters = ommittedCounters;

		return itsTime;
	}

private:
	// licznik liczacy w gore i w dol od 1 do zadanej wartosci
	void CalculationForUpDownCounter(unsigned int& outCounter, unsigned int* outPeriod)
	{
		/*
			this->counter to licznik odliczajacy od 1 do nieskonczonosci
			outCounter to licznik odliczajacy od 1 do countTo
			modulo trzeba robic na liczbach od zera, a counter jest od 1
			dlatego przed operacja modulo trzeba od countera odjac 1 i do wyniku dodac 1

			niech countTo = 4
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
		// wyznaczamy wartosc licznika
		outCounter = (counter - 1) % (countTo - 1);
		bool fallingSlope = (counter - 1) / (countTo - 1) % 2 == 1;
		if (fallingSlope)
			if (outCounter == 0)
				outCounter = countTo;
			else
				outCounter = countTo - outCounter;
		else
			outCounter = outCounter + 1;

		// wyznaczamy okres
		if (outPeriod)
			*outPeriod = (counter - 1) / (2 * countTo - 2) + 1;
	}

	// wylapywanie pominietych wartosci min i max
	void CatchMinMaxUpDown(unsigned int& outCounter, unsigned int* outPeriod, unsigned int ommittedCounters)
	{
		// dla cykniecia 1 i max nie ma czego sprawdzac
		if (outCounter == 1 || outCounter == countTo)
			return;

		// sprawdzamy czy zostalo pominiete 1 lub max
		bool fallingSlope = (counter - 1) / (countTo - 1) % 2 == 1;
		if (fallingSlope)
		{
			if (ommittedCounters >= countTo - outCounter)
			{
				// zostalo pominiete max - cofamy licznik do max
				//Serial.println("catched ommited max");
				counter -= countTo - outCounter;
				outCounter = countTo;
				// TODO: ew. cofniecie *outPeriod
			}
		}
		else // raisingSlope
		{
			if (ommittedCounters >= outCounter - 1)
			{
				// zostalo pominiete 1 - cofamy licznik do 1
				//Serial.println("catched ommited 1");
				counter -= outCounter - 1;
				outCounter = 1;
				// TODO: ew. cofniecie *outPeriod
			}
		}
	}
};

// ----------------------------
#pragma region OLD

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

// Licznik odliczajacy okresowo od 0 do zadanej wartosci
class CounterPeriodicOLD : public Counter
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
			*period = 0;
		if (outOmmittedCounters)
			*outOmmittedCounters = 0;

		// counterTo musi byc podany
		if (countTo <= 1 && IsStarted())
		{
			Serial.println(F("* CounterPeriodic::counterTo=0!"));
			Stop();
			return false;
		}

		// czy naszedla czas aktywacji licznika?
		unsigned int ommittedCounters;
		bool elapsed = Counter::ItsTime(Counter::Options::WithZero, counter, &ommittedCounters);
		if (elapsed)
		{
			// tak, nadszedl czas aktywacji
			// na podstawie licznika liczacego do nieskonczonosci wyznaczamy wartosc licznika okresowego
			counter = this->counter % (countTo + 1);
		}

		// wartosci zwrotne
		if (period)
			*period = this->counter / (countTo + 1);
		if (outOmmittedCounters)
			*outOmmittedCounters = ommittedCounters;

		return elapsed;
	}
};

#pragma endregion
