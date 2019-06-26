#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <pixeltypes.h>

class Strip
{
public:
	int ledsCount = 180;
	CRGB *leds = nullptr;

	Strip();
	~Strip();
	void Loop();

private:
	int ledCurrent = 0;
};
