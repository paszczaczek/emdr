#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <ArduinoSTL.h>
#include "StripPlugin.h"

class Strip
{
public:
	int ledsCount = 0;
	CLEDController *ledsController = nullptr;

	~Strip();

	template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
	void SetController(int ledsCount)
	{
		leds = new CRGB[ledsCount];
		this->ledsCount = ledsCount;
		ledsController = &FastLED.addLeds<CHIPSET, DATA_PIN, RGB_ORDER>(leds, ledsCount);
		ledsController->clearLedData();
		//uint8_t max_brightness = calculate_max_brightness_for_power_vmA(
		//	ledsController->leds(), ledsController->size(), 255, 5, 10);
		//Serial.print("max brightness: ");
		//Serial.println(max_brightness);
	}

	void AddPlugin(StripPlugin *plugin);
	void Loop();

private:
	CRGB *leds = nullptr;
	int ledCurrent = 0;
	std::vector<StripPlugin*> plugins;
};
