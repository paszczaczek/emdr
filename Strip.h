#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "StripPlugin.h"
#include "Device.h"
#include "Storage.h"

extern class DiagnosticStipPlugin diagnosticStipPlugin;
extern class MovingPointStripPlugin movingPointStripPlugin;

class Strip : public Device
{
private:
	constexpr static bool turnOnBuiltinLedOnStripUpdate = false;
	constexpr static void* plugins[] = {
		&diagnosticStipPlugin,
		&movingPointStripPlugin
	};

public:
	// konstruktor
	Strip(int maxCurrent)
	{
		FastLED.setMaxPowerInVoltsAndMilliamps(5, maxCurrent);
	}

	// konfiguracja paska
	void Setup()
	{
		// zgaszenie diody L, bo ciagle swieci i przeszkadza w nocy
		pinMode(LED_BUILTIN, OUTPUT);
		digitalWrite(LED_BUILTIN, LOW);
		ledsBrightness = storage.movingPointStripPlugin_Brightness;
		updated = true;
	}

	// konfiguracja kontrolera fastled
	template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
	void SetController()
	{
		pinMode(DATA_PIN, OUTPUT);
		controller = &FastLED.addLeds<CHIPSET, DATA_PIN, RGB_ORDER>(leds, ledsCount);
		FastLED.clear(true);
		FastLED.setBrightness(0);
	}

	// zwrocenie pluginu poruszajacego sie punktu
	StripPlugin* GetMovingPointStripPlugin() {
		return (StripPlugin*)&movingPointStripPlugin;
	}

	// zwrocenie pluginu diagnostycznego
	StripPlugin* GetDiagnosticStipPlugin() {
		return (StripPlugin*)&diagnosticStipPlugin;
	}

	// zatrzymanie wszystkich pluginow
	void StopAllPlugins()
	{
		((StripPlugin*)&diagnosticStipPlugin)->Stop();
		((StripPlugin*)&movingPointStripPlugin)->Stop();
	}

	// obsluga petli zdarzen
	void Loop() override
	{
		ForEachPlugins([](StripPlugin* plugin, Event::Name)
			{
				if (plugin->state != Plugin::State::Stopped)
					plugin->Loop();
			});

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

	bool Receive(Event::Name eventReceived) override
	{
		ForEachPlugins([](StripPlugin* plugin, Event::Name eventReceived)
			{
				plugin->Receive(eventReceived);

			}, eventReceived);

		return false;
	}

	// ustawienie wszystkich ledow na wskazany kolor
	void SetAllLeds(CRGB color)
	{
		for (int i = 0; i < controller->size(); i++)
			controller->leds()[i] = color;
	}

	void SetBrightness(uint8_t scale)
	{
		ledsBrightness = scale;
	}

	// czy stan strpu ulegl zmianie i trzeba odswiezyc diody
	bool updated = false;

	// kontroler ledow
	CLEDController* controller = nullptr;

	// jasnosc swiecenia wszystkich diod
	byte ledsBrightness;

private:

	// wykonuje akcje dla wszystkich pluginow
	void ForEachPlugins(
		void (*action)(StripPlugin*, Event::Name), 
		Event::Name eventReceived = Event::Name::UnknowCode)
	{
		for (size_t i = 0; i < pluginsCount; i++)
		{
			StripPlugin* plugin = (StripPlugin*)plugins[i];
			(*action)(plugin, eventReceived);
		}
	}

private:
	constexpr static int ledsCount = 179;
	constexpr static int diagnosticStipPluginIdx = 0;
	constexpr static int movingPointStripPluginIdx = 1;
	constexpr static int pluginsCount = sizeof(plugins) / sizeof(*plugins);
	CRGB leds[ledsCount];
};

extern Strip strip;