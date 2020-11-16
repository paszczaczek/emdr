#pragma once
#include <Arduino.h>
#include <stdio.h>
//#include "Event.h"

#define DEBUG_TIMER_ITSTIME 0

class Timer
{
public:

	// Czas trwania interwalu.
	enum class Interval : byte {
		ms1 = 0, ms2, ms4, ms8, ms16, ms32, ms64, ms128, ms256, ms512, ms1024,
		s2, s4, s8, s16, s32, s64,
		m2, m4, m8, m16, m32, m64,
		h2, h4, h8, h16, h32, h64, h128,

		s1 = ms1024,
		m1 = s64,
		h1 = m64,
	};

	// Pojemnosc licznika.
	enum class Capacity : byte {
		bit1 = 1, 
		bits2,  
		bits3,  
		bits4,  
		bits5,  
		bits6,  
		bits7,  
		bits8,  
		bits9,  
		bits10, 
		bits11, 
		bits12, 
		bits13, 
		bits14, 
		bits15, 
		bits16, 
		bits17, 
		bits18, 
		bits19, 
		bits20, 
		bits21, 
		bits22, 
		bits23, 
		bits24, 
		bits25, 
		bits26, 
		bits27, 
		bits28, 
		bits29, 
		bits30, 
		bits31, 
		bits32, 

		values2 = bit1,
		values4 = bits2,
		values8 = bits3,
		values16 = bits4,
		values32 = bits5,
		values64 = bits6,
		values128 = bits7,
		values256 = bits8,
		values512 = bits9,
		values1024 = bits10,
		values2k = bits11,
		values4k = bits12,
		values8k = bits13,
		values16k = bits14,
		values32k = bits15,
		values65k = bits16,
		values131k = bits17,
		values262k = bits18,
		values524k = bits19,
		values1048k = bits20,
		values2m = bits21,
		values4m = bits22,
		values8m = bits23,
		values16m = bits24,
		values33m = bits25,
		values67m = bits26,
		values134m = bits27,
		values268m = bits28,
		values536m = bits29,
		values1073m = bits30,
		values2g = bits31,
		values4g = bits32
	};

	// Czy nadszel czas wystrzelenia licznika.
	static bool ItsTime(
		Interval interval,
		Capacity capacity,
		unsigned long* startedAt,
		unsigned long countTo,
		unsigned long* ommitted,
		char label = '\0')
	{
		/*
		...x xx.. .... ....   window, resolution=1024ms, capacity=3bity
		0 1 2 3 4 5 6 7       dostepne wartosci "czasu"
				  S-->N       przypadek 1: elapsed = 7 - 5 = 2
		----->N   S----       przypadek 2: elapsed = (7 - 5) + 1 + (3 - 0) = 6
		*/

		if (!IsStarted(countTo))
			return false;

		unsigned long now = Now(interval, capacity);
		unsigned long max = Window(interval, capacity, -1);
		unsigned long elapsed;
		// przypadek 1
		if (now >= *startedAt)
			elapsed = now - *startedAt;
		// przypadek 2
		else
			elapsed = (max - *startedAt) + 1 + now;

		if (elapsed >= countTo)
		{
			if (ommitted)
				*ommitted = elapsed - countTo;
#if DEBUG_TIMER_ITSTIME
			if (label)
			{
				static unsigned long prevMs = 0;
				unsigned long nowMs = millis();
				unsigned long elapsedMs = nowMs - prevMs;
				unsigned long shouldElapseMs = (1 << (byte)resolution) * countTo;
				char buf[256] = "";
				snprintf(buf, sizeof(buf),
					"%c ms:%ld->%ld/%ld s:%ld n:%ld/%ld e:%ld/%ld o:%ld",
					label,
					nowMs, elapsedMs, shouldElapseMs,
					*startedAt,
					now, max,
					elapsed, countTo, ommitted ? *ommitted : -1);
				Serial.println(buf);
				prevMs = nowMs;
			}
#endif
			* startedAt = now;
			return true;
		}
		else
		{
			return false;
		}

#if 0
		if (!IsStarted(countTo))
			return false;

		unsigned long nowMs = millis();
		unsigned long startedAtMs = *startedAt << (byte)interval;
		unsigned long elapsedMs = nowMs - startedAtMs;
		//unsigned long elapsedIntervals = Window(interval, capacity, elapsedMs);
		*omittedIntervals = elapsedMs / (1 << (byte)interval);
		if (*omittedIntervals >= countTo)
		{
			*startedAt = Window(interval, capacity, nowMs);
#if DEBUG_TIMER_ITSTIME
			static unsigned long prev = 0;
			if (label)
			{
				const char tmp[2] = { label, 0 };
				Serial.print(tmp);
				Serial.print(F(" "));
			}
			Serial.print(millis());
			Serial.print(F(" "));
			Serial.print(*omittedIntervals);
			Serial.print(F(" "));
			Serial.println(nowMs - prev);
			prev = nowMs;
#endif
			return true;
		}
		else
		{
			return false;
		}
#endif
	}

	static unsigned long Now(Interval interval, Capacity capacity)
	{
		return Window(interval, capacity, millis());
	}


	static unsigned long Window(Interval interval, Capacity capacity, unsigned long ms)
	{
		// ........ ........ ..XXXXXX X......  - window i millis()
		byte shiftLeft = (byte)interval;
		byte windowWidth = (byte)capacity;

		if (shiftLeft + windowWidth > 32)
		{
			Serial.println(F("*err Timer::Now"));
			return 0;
		}

		byte shiftRight = 32 - shiftLeft - windowWidth;

		unsigned long window = ms;
		//Serial.print(shiftRight); Serial.print(F(" ")); Serial.print(window); Serial.print(F(" -> "));
		window <<= shiftRight;
		window >>= shiftRight;
		window >>= shiftLeft;
		//Serial.println(window);

		return window;
	}

	// Czy nadszedl czas na wystrzelenie timera?
	static bool ItsTime(
		Interval resolution,
		Capacity capacity,
		unsigned long& startedAt)
	{
		return true;
	}

	// Uruchomienie timera.
	static unsigned long Start(Interval resolution, Capacity capacity)
	{
		return Now(resolution, capacity);
	}

	// Zatrzymanie timera.
	static unsigned long Stop()
	{
		return -1;
	}

	// Czy timer jest uruchomiony?
	static bool IsStarted(unsigned long countTo)
	{
		return countTo != 0;
	}
};