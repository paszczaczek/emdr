#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <ArduinoSTL.h>
#include "StripPlugin.h"

class Strip
{
public:
	CLEDController *controller = nullptr;
	bool updated = false;

	template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
	void SetController(int ledsCount)
	{
		pinMode(DATA_PIN, OUTPUT);
		leds = new CRGB[ledsCount];
		controller = &FastLED.addLeds<CHIPSET, DATA_PIN, RGB_ORDER>(leds, ledsCount);
		controller->clearLedData();
	}

	~Strip();
	void AddPlugin(StripPlugin *plugin);
	void Loop();

private:
	CRGB *leds = nullptr;
	int ledCurrent = 0;
	std::vector<StripPlugin*> plugins;
};
