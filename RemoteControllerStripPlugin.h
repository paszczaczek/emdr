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

		// ustaw timer do migania diod¹ po wciœniêciu guzika na pilocie
		flashTimer.elapsed += new EventHandler<RemoteControllerStripPlugin, Timer::EventArgs>
			(this, &RemoteControllerStripPlugin::onTimerFlashEvent);
		flashTimer.interval = 1000;
		flashTimer.autoReset = false;

		buttonPressed = false;
		buttonUnsupported = false;
	}

	virtual void Loop() override
	{
		// utwórz kontroler do aktualizacji ledsCount pierwszych diod obsugiwanych przez plugin pilota
		// !!!
		// https://github.com/FastLED/FastLED/issues/280
		// Daniel Garcia: you can't have multiple controllers on the same pin
		if (!controller)
			controller = &FastLED.addLeds<CHIPSET, DATA_PIN+1, RGB_ORDER>
			(strip->controller->leds(), ledsCount);

		flashTimer.Loop();
		updateLeds();
	}

private:
	CLEDController *controller = nullptr;
	const uint8_t ledsCount = 1;
	const CRGB unset = CRGB::Beige;
	Timer flashTimer;
	uint8_t buttonPressed : 1;
	uint8_t buttonUnsupported : 1;

	void onRemoteControllerButtonPressed(RemoteController::EventArgs &args)
	{
		// wciœniêto guzik na pilocie
		buttonPressed = true;
		buttonUnsupported = args.button == RemoteController::Button::UNSUPPORTED;
		flashTimer.Start();
	}

	void onTimerFlashEvent(Timer::EventArgs& args)
	{
		// czas zgasiæ diodê sygnalizuj¹c¹ wciœniêcie guzika na pilocie
		flashTimer.Stop();
		buttonPressed = false;
		buttonUnsupported = false;
	}

	void updateLeds()
	{
		// wyznacz kolor jaki ma miec dioda sygnalizujaca wcisniecie guzika na pilocie
		CRGB led0HasToBe;
		if (buttonPressed)
			led0HasToBe = buttonUnsupported ? CRGB::Blue : CRGB::Red;
		else
			led0HasToBe = CRGB::Violet;

		// czy kolor jest inny niz aktualny
		CRGB &led0current = controller->leds()[0];
		if (led0HasToBe.r != led0current.r ||
			led0HasToBe.g != led0current.g ||
			led0HasToBe.b != led0current.b)
		{
			led0current = led0HasToBe;
			controller->showLeds(2);
		}
	}
};
