#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>
//#include <pixeltypes.h>
#include "Timer.h"
#include "EventArgs.h"

class Strip
{
public:
	CRGB *leds = nullptr;
	int ledsCount = 180;
	CLEDController *ledsController;

	Strip();
	~Strip();

	template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
	void SetController(int ledsCount)
	{
		leds = new CRGB[ledsCount];
		memset(leds, 0, sizeof(CRGB) * ledsCount);
		this->ledsCount = ledsCount;
		ledsController = &FastLED.addLeds<CHIPSET, DATA_PIN, RGB_ORDER>(leds, ledsCount);
		ledsController->showColor(CRGB::Black);
	}

	void Loop();

private:

	uint8_t ledCurrent = 0;
	enum MovingDirection { RIGTH, LEFT } movingDirection = RIGTH;;

	Timer movingTimer;
	void onMovingEvent(TimerEventArgs& args);
};
