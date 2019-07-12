#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <ArduinoSTL.h>
#include "StripPlugin.h"

class Strip
{
public:
	template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
	void SetController(int ledsCount)
	{
		pinMode(DATA_PIN, OUTPUT);
		leds = new CRGB[ledsCount];
		controller = &FastLED.addLeds<CHIPSET, DATA_PIN, RGB_ORDER>(leds, ledsCount);
		controller->clearLedData();
	}

	void AddPlugin(StripPlugin *plugin)
	{
		plugin->strip = this;
		plugins.push_back(plugin);
	}

	void Loop()
	{
		for (auto plugin : plugins)
			plugin->Loop();

		if (updated)
		{
			// controller->showLeds() nie uwzglednia FastLED.setMaxPowerInVoltsAndMilliamps() !
			//controller->showLeds(2);
			FastLED.show(2);
			updated = false;
		}
	}

	~Strip()
	{
		for (auto plugin : plugins)
			delete plugin;

		delete[] leds;
	}

	bool updated = false;
	CLEDController *controller = nullptr;

private:
	CRGB *leds = nullptr;
	int ledCurrent = 0;
	std::vector<StripPlugin*> plugins;
};
