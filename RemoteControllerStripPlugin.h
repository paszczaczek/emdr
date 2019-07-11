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
		for (int i = 0; i < ledsCount; i++)
			leds[i] = unset;

		remoteController.buttonPressed +=
		new EventHandler<RemoteControllerStripPlugin, RemoteController::EventArgs>
			(this, &RemoteControllerStripPlugin::onRemoteControllerButtonPressed);

		flashTimer.elapsed += new EventHandler<RemoteControllerStripPlugin, Timer::EventArgs>
			(this, &RemoteControllerStripPlugin::onTimerFlashEvent);
		flashTimer.interval = 1000;
		flashTimer.autoReset = false;
	}

	virtual void Loop() override
	{
		if (!controller)
			controller = &FastLED.addLeds<CHIPSET, DATA_PIN, RGB_ORDER>
				(strip->controller->leds(), ledsCount);

		if (strip->updated)
			updateLeds();

		flashTimer.Loop();
	}

private:
	const CRGB unset = CRGB::Beige;
	CLEDController *controller = nullptr;
	static const uint8_t ledsCount = 1;
	CRGB leds[ledsCount];
	Timer flashTimer;

	void onRemoteControllerButtonPressed(RemoteController::EventArgs &args) 
	{
		leds[0] 
			= args.button == RemoteController::Button::UNSUPPORTED
			? CRGB::Blue
			: CRGB::Red;
		updateLeds();
		flashTimer.Start();
	}

	void onTimerFlashEvent(Timer::EventArgs& args)
	{
		flashTimer.Stop();
		leds[0] = CRGB::Black;
		updateLeds();
		leds[0] = unset;
	}

	void updateLeds()
	{
		for (int i = 0; i < ledsCount; i++)
		{
			if (leds[i].r == unset.r
				&& leds[i].g == unset.g
				&& leds[i].b == unset.b)
				continue;
			controller->leds()[i] = leds[i];
			strip->updated = true;
		}
	}
};
