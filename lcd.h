#pragma once
#include "Arduino.h"
#include <LCD_I2C.h>
//#include "Print.h"
//#include "Printable.h"

class Lcd : public LCD_I2C
{
public:
	Lcd() : LCD_I2C(0x27) {}

	void backlight()
	{
		LCD_I2C::backlight();
		TimerStart();
	}
	
	size_t print(const __FlashStringHelper* flash)
	{
		TimerStart();
		return LCD_I2C::print(flash);
	}

#if !WIN32
	size_t print(const char* s)
	{
		TimerStart();
		return LCD_I2C::print(s);
	}
#endif

	size_t print(char c)
	{
		TimerStart();
		return LCD_I2C::print(c);
	}

	size_t print(unsigned char c, int format = DEC)
	{
		TimerStart();
		return LCD_I2C::print(c, format);
	}

	size_t print(int i, int format = DEC)
	{
		TimerStart();
		return LCD_I2C::print(i, format);
	}

	size_t printProp(const __FlashStringHelper* name, const byte value)
	{
		TimerStart();
		return
			print(name) +
			print(':') +
			print(' ') +
			print(value);
	}

	size_t printPropPerc(const __FlashStringHelper* name, const byte value)
	{
		TimerStart();
		return
			printProp(name, value) +
			print('%');
	}

	void setup()
	{}

	void loop()
	{
		// czy minal czas po ktorym nalezy zgasic backlight
		unsigned long startedAt = timerStartedAt;
		if (Timer::ItsTime(
			timerResolution, timerCapacity,
			&startedAt, timerCountTo,
			NULL))
		{
			TimerStop();
		}
	}

	class PropertyValue : public Printable
	{
	public:
		PropertyValue(const __FlashStringHelper* name, const byte value)
			: name(name), value(value) {}

		virtual size_t printTo(Print& p) const
		{
			return
				p.print(name) +
				p.print(':') +
				p.print(' ') +
				p.print(value);
		}
	public:
		const __FlashStringHelper* name;
		const byte value;
	};

	class PropertyPercent : public PropertyValue
	{
	public:
		PropertyPercent(const __FlashStringHelper* name, const byte percent)
			: PropertyValue(name, percent) {}

		size_t printTo(Print& p) const
		{
			return
				PropertyValue::printTo(p) +
				p.print(F("%  "));
		}
	};

private:
	// rozdzielczosc timera
	constexpr static Timer::Interval timerResolution = Timer::Interval::s1;

	// pojemnosc timera
	constexpr static Timer::Capacity timerCapacity = Timer::Capacity::bits2;

	// czas wystartowania timera
	byte timerStartedAt : static_cast<int>(timerCapacity);

	// po ilu interwalach wystrzelic timer
	byte timerCountTo : static_cast<int>(timerCapacity);

	// uruchom timer wylaczania backlight
	void TimerStart()
	{
		timerStartedAt = Timer::Start(timerResolution, timerCapacity);
		timerCountTo = 3;
		LCD_I2C::backlight();
	}

	// zatrzymaj timer wylaczania backlight
	void TimerStop()
	{
		timerStartedAt = Timer::Stop();
		timerCountTo = 0;
		LCD_I2C::noBacklight();
	}

};

extern Lcd lcd;