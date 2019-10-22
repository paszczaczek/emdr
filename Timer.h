#pragma once
#include <Arduino.h>
#include <stdio.h>
//#include "Event.h"

// Klasa bazowa dla Timerow i Counterow, wylicza czasy aktywacji
class Timer
{
public:
	// po ilu milisekundach aktywowac timer
	unsigned long interval = 0;

	// warianty timera
	enum struct Mode : byte { SingleShot, MultiShot };

protected:
	// czas uruchomienia timera w milisekundach
	unsigned long startedAt = (unsigned long)-1;

public:
	// uruchomienie timera
	void Start()
	{
		startedAt = millis();
	}

	// zatrzymanie timera
	void Stop()
	{
		startedAt = (unsigned long)-1;
	}

	// czy timer jest uruchomiony
	bool IsStarted()
	{
		return startedAt != (unsigned long)-1;
	}

	// czy nadszedl czas wystrzelenia timera
	bool ItsTime(Mode mode, unsigned int* outOmmittedIntervals = NULL)
	{
		// inicjalizacja wartosci zwrotnych
		if (outOmmittedIntervals)
			*outOmmittedIntervals = 0;

		// czy timer zostal uruchomiony?
		if (!IsStarted())
			return false;

		// interval musi byc podany
		if (interval == 0)
		{
			Serial.println(F("* Timer::interval=0!"));
			Stop();
			return false;
		}

		// ile minelo pelnych intervalow od momentu startu
		unsigned int elapsedIntervals = (millis() - startedAt) / interval;

		// ile interwalow zostalo pominietych
		unsigned int ommittedIntervals 
			= elapsedIntervals > 1
			? elapsedIntervals - 1 
			: 0;

		// zwrocenie liczby pominietych intervalow
		if (outOmmittedIntervals)
			*outOmmittedIntervals = ommittedIntervals;

		// czy nadszel czas wystrzelenia timera
		if (elapsedIntervals == 0)
			return false;

		// tak, nadszedl czas wystrzelenia timera
		switch (mode)
		{
		case Timer::Mode::SingleShot:
			// to timer jednorazowy - stopujemy go
			Stop();
			break;
		case Timer::Mode::MultiShot:
			// to timer wielorazowy - uruchamiamy ponownie
			// korygujac moment startu ze wzgledu na pominiete interwaly
			startedAt += (1 + ommittedIntervals) * interval;
			break;
		}

		return true;
	}
};

// Licznik w roznych warianach
class Counter : public Timer
{
public:
	// warianty licznika
	enum Mode : byte { Up, Down };

	// opcje licznika
	enum Options : byte { None = 0, WithZero = 1 };

protected:
	// ile minelo interwalow od czasu wystartowania licznika
	unsigned int counter = (unsigned int)-1;

public:
	// zatrzymanie licznika
	void Stop()
	{
		Timer::Stop();
		counter = (unsigned int)-1;
	}

	// zapauzowanie licznika
	void Pause()
	{
		Timer::Stop();
	}

	// wznowienie licznika
	void Resume()
	{
		Timer::Start();
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
		bool elapsed = Timer::ItsTime(
			Timer::Mode::MultiShot,
			&ommitedCounters);
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
	enum Options : byte { None = 0, WithZero = 1, CatchMinMax = 2 };

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
			static_cast<Counter::Options>(options),
			counter,
			&ommittedCounters);

		// jesli nie to konczymy
		if (!itsTime || counter == 0)
			return itsTime;

		// nadszedl czas aktywacji licznika, wyznaczamy wartosci licznika i okresu
		switch (mode)
		{
		case CounterPeriodic::UpDown: CalculationForUpDownCounter(outCounter, outPeriod); break;
		case CounterPeriodic::Up:
		case CounterPeriodic::Down:
		case CounterPeriodic::DownUp: Serial.println(F("* Not implemented yet!")); Stop();  return false;
		}

		// wylapanie ominietego 1 lub max
		if (options & Options::CatchMinMax)
			CatchMinMaxUpDown(outCounter, outPeriod, ommittedCounters);

		if (outOmmittedCounters)
			*outOmmittedCounters = ommittedCounters;

		return itsTime;
	}

private:
	// obliczenia wartosci licznika i okresu dla licznika licz¹cego okresowo w gore i w dol od 1 do zadanej wartosci
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

// operator bitowy | dla enum Counter::Options
inline Counter::Options operator|(Counter::Options a, Counter::Options b)
{
	return static_cast<Counter::Options>(static_cast<byte>(a) | static_cast<byte>(b));
}

// operator bitowy | dla enum CounterPeriodic::Options
inline CounterPeriodic::Options operator|(CounterPeriodic::Options a, CounterPeriodic::Options b)
{
	return static_cast<CounterPeriodic::Options>(static_cast<byte>(a) | static_cast<byte>(b));
}