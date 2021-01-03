#pragma once
#include "Arduino.h"
#include "Print.h"
#include "Printable.h"

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