#pragma once
#include <inttypes.h>
#include "StripPlugin.h"
#include "RemoteController.h"
#include "Emdr.h"
#include "EventHandler.h"
#include "Timer.h"

template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
class RemoteControllerStripPlugin : public StripPlugin
{
public:
	RemoteControllerStripPlugin()
	{
		// subskrybuj eventy od pilota
		remoteController.buttonPressed +=
			new EventHandler<RemoteControllerStripPlugin, RemoteController::EventArgs>
			(this, &RemoteControllerStripPlugin::onRemoteControllerButtonPressed);

		// ustaw timer do migania diode po wcisnieciu guzika na pilocie
		flashTimer.elapsed += new EventHandler<RemoteControllerStripPlugin, Timer::EventArgs>
			(this, &RemoteControllerStripPlugin::onFlashTimerEvent);
		flashTimer.autoReset = false;

		// power on test - zaswiecamy wszystkie diody
		buttonIndicator = (uint8_t)BUTTON_INDICATOR::ON;
		buttonUnsupported = false;
		flashTimer.interval = 2000;
		flashTimer.Start();
	}

	virtual void Loop() override
	{
		// utw�rz kontroler do aktualizacji ledsCount pierwszych diod obsugiwanych przez plugin pilota
		// !!!
		// https://github.com/FastLED/FastLED/issues/280
		// Daniel Garcia: you can't have multiple controllers on the same pin
		if (!controller)
			controller = &FastLED.addLeds<CHIPSET, DATA_PIN, RGB_ORDER>
			(strip->controller->leds(), ledsCount);

		flashTimer.Loop();
		updateLeds();
	}

private:
	CLEDController *controller = nullptr;
	const uint8_t ledsCount = 1;
	Timer flashTimer;

	enum class BUTTON_INDICATOR :uint8_t { UNSET = 0, ON = 1, OFF = 2};

	uint8_t buttonIndicator : 2;
	uint8_t buttonUnsupported : 1;

	void onRemoteControllerButtonPressed(RemoteController::EventArgs &args)
	{
		// wcisnieto guzik na pilocie
		//Serial.print("onRemoteControllerButtonPressed: ");
		//Serial.println(args.button);
		buttonIndicator = (uint8_t)BUTTON_INDICATOR::ON;
		buttonUnsupported = args.button == RemoteController::Button::UNSUPPORTED;
		flashTimer.interval = 90;
		flashTimer.Start();
	}

	void onFlashTimerEvent(Timer::EventArgs& args)
	{
		(void) args;
		// czas zgasic diode sygnalizujaca wcisniecie guzika na pilocie
		flashTimer.Stop();
		buttonIndicator = (uint8_t)BUTTON_INDICATOR::OFF;
	}

	void updateLeds()
	{
		// wyznacz kolor jaki ma miec dioda sygnalizujaca wcisniecie guzika na pilocie
		CRGB biColor = CRGB::Black;
		switch (buttonIndicator) {
		case (uint8_t)BUTTON_INDICATOR::ON: 
			// wcisnieto guzik na pilocie - dioda ma sie swiecic
			biColor = buttonUnsupported ? CRGB::Blue : CRGB::Red; 
			break;
		case (uint8_t)BUTTON_INDICATOR::OFF:
			// wcisnieto guzik na pilocie ale czas swiecenia diody minal - wylaczyc diode
			biColor = CRGB::Black;
			buttonIndicator = (uint8_t)BUTTON_INDICATOR::UNSET;
			break;
		case (uint8_t)BUTTON_INDICATOR::UNSET:
			// zaden guzik na pilocie nie byl wciskany - nie ingerujemy w swiecenie diody
			return;
		}

		// czy kolor zmienil sie?
		CRGB &biColorCurrent = controller->leds()[0];
		if (biColor.r != biColorCurrent.r ||
			biColor.g != biColorCurrent.g ||
			biColor.b != biColorCurrent.b)
		{
			// tak
			biColorCurrent = biColor;
			// na razie zablokuje, do czasu gdy wyjasni sie czy potrzebny bedzie osobny strip dla remotecontrollera
			// controller->showLeds(2);
			strip->updated = true;
		}
	}
};
