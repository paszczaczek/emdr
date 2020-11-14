#pragma once
//#define FASTLED_ALLOW_INTERRUPTS 1
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "StripPlugin.h"
#include "RemoteController.h"
#include "Device.h"

class Strip : public Device
{
public:
	Strip(int maxCurrent, byte ledsBrightness) :
		ledsBrightness(ledsBrightness)
	{
		FastLED.setMaxPowerInVoltsAndMilliamps(5, maxCurrent);
	}
	
	// konfiguracja paska ledow
	template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
	void SetController()
	{
		pinMode(DATA_PIN, OUTPUT);
		controller = &FastLED.addLeds<CHIPSET, DATA_PIN, RGB_ORDER>(leds, ledsCount);
		FastLED.clear(true);
	}

	// dodanie pluginu poruszajacego sie punktu
	void AddMovingPointStripPlugin(StripPlugin* movingPointStripPlugin)
	{
		plugins[movingPointStripPluginIdx] = movingPointStripPlugin;
	}

	// zwrocenie pluginu poruszajacego sie punktu
	StripPlugin* GetMovingPointStripPlugin() {
		return plugins[movingPointStripPluginIdx];
	}

	// dodanie pluginu diagnostycznego
	void AddDiagnosticStipPlugin(StripPlugin* diagnosticStipPlugin)
	{
		plugins[diagnosticStipPluginIdx] = diagnosticStipPlugin;
	}

	// zwrocenie pluginu diagnostycznego
	StripPlugin* GetDiagnosticStipPlugin() {
		return plugins[diagnosticStipPluginIdx];
	}

	void StartAllPlugins()
	{
		for (byte i = 0; i < pluginsCount; i++)
			plugins[i]->Start();
	}

	void StopAllPlugins()
	{
		for (byte i = 0; i < pluginsCount; i++)
			plugins[i]->Stop();
	}

	void Loop() override
	{
		for (byte i = 0; i < pluginsCount; i++)
			if (plugins[i]->state != Plugin::State::Stopped)
				plugins[i]->Loop();

		if (updated)
		{
			FastLED.show(ledsBrightness);
			updated = false;
		}
	}

	bool Receive(Event::Name eventName) override
	{
		for (byte i = 0; i < pluginsCount; i++)
			plugins[i]->Receive(eventName);

		return false;
	}

	void SetLeds(CRGB color)
	{
		for (int i = 0; i < controller->size(); i++)
			controller->leds()[i] = color;
	}

	bool updated = false;
	CLEDController* controller = nullptr;
	byte ledsBrightness;

private:
	constexpr static int ledsCount = 179;
	constexpr static int pluginsCount = 2;
	constexpr static int diagnosticStipPluginIdx = 0;
	constexpr static int movingPointStripPluginIdx = 1;
	CRGB leds[ledsCount];
	StripPlugin* plugins[pluginsCount];
};

extern Strip strip;