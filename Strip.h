#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "StripPlugin.h"
#include "Device.h"

extern class DiagnosticStipPlugin diagnosticStipPlugin;
extern class MovingPointStripPlugin movingPointStripPlugin;

class Strip : public Device
{
private:
	constexpr static bool turnOnBuiltinLedOnStripUpdate = false;

public:
	Strip(int maxCurrent, byte ledsBrightness) :
		ledsBrightness(ledsBrightness)
	{
		FastLED.setMaxPowerInVoltsAndMilliamps(5, maxCurrent);
	}

	// konfiguracja paska
	void Setup()
	{
		// zgaszenie diody L, bo ciagle swieci i przeszkadza w nocy
		pinMode(LED_BUILTIN, OUTPUT);
		digitalWrite(LED_BUILTIN, LOW);
	}

	// konfiguracja kontrolera fastled
	template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
	void SetController()
	{
		pinMode(DATA_PIN, OUTPUT);
		controller = &FastLED.addLeds<CHIPSET, DATA_PIN, RGB_ORDER>(leds, ledsCount);
		FastLED.clear(true);
	}

	// zwrocenie pluginu poruszajacego sie punktu
	StripPlugin* GetMovingPointStripPlugin() {
		return (StripPlugin*)&movingPointStripPlugin;
	}

	// zwrocenie pluginu diagnostycznego
	StripPlugin* GetDiagnosticStipPlugin() {
		return (StripPlugin*)&diagnosticStipPlugin;
	}

	void StartAllPlugins()
	{
		((StripPlugin*)&diagnosticStipPlugin)->Start();
		((StripPlugin*)&movingPointStripPlugin)->Start();
	}

	void StopAllPlugins()
	{
		((StripPlugin*)&diagnosticStipPlugin)->Stop();
		((StripPlugin*)&movingPointStripPlugin)->Stop();
	}

	void Loop() override
	{
		auto diagnostic = (StripPlugin*)&diagnosticStipPlugin;
		if (diagnostic->state != Plugin::State::Stopped)
			diagnostic->Loop();

		auto movingPoint = (StripPlugin*)&movingPointStripPlugin;
		if (movingPoint->state != Plugin::State::Stopped)
			movingPoint->Loop();

		if (updated)
		{
			if constexpr (turnOnBuiltinLedOnStripUpdate)
				digitalWrite(LED_BUILTIN, HIGH);
			FastLED.show(ledsBrightness);
			if constexpr (turnOnBuiltinLedOnStripUpdate)
				digitalWrite(LED_BUILTIN, LOW);
			updated = false;
		}
	}

	bool Receive(Event::Name eventName) override
	{
		auto diagnostic = (StripPlugin*)&diagnosticStipPlugin;
		diagnostic->Receive(eventName);

		auto movingPoint = (StripPlugin*)&movingPointStripPlugin;
		movingPoint->Receive(eventName);

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
	constexpr static int diagnosticStipPluginIdx = 0;
	constexpr static int movingPointStripPluginIdx = 1;
	CRGB leds[ledsCount];
};

extern Strip strip;