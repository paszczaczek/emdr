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
	Strip(CRGB* leds, byte ledsCount, StripPlugin** plugins, byte pluginsCount, int maxCurrent, byte ledsBrightness) :
		ledsBrightness(ledsBrightness),
		leds(leds),
		ledsCount(ledsCount),
		plugins(plugins),
		pluginsCount(pluginsCount)
	{
		FastLED.setMaxPowerInVoltsAndMilliamps(5, maxCurrent);
		//remoteControllerEventHandler.Set(this, &Strip::OnRemoteControllerEvent);
		//remoteController.event += remoteControllerEventHandler;
	}

	template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
	void SetController()
	{
		pinMode(DATA_PIN, OUTPUT);
		controller = &FastLED.addLeds<CHIPSET, DATA_PIN, RGB_ORDER>(leds, ledsCount);
		FastLED.clear(true);
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

	//void OnRemoteControllerEvent(RemoteController::EventArgs &args)
	//{
	//    for (byte i = 0; i < pluginsCount; i++)
	//        if (plugins[i]->state != Plugin::State::Stopped)
	//            plugins[i]->OnRemoteControllerEvent(args);
	//            /*switch (args.button)
	//            {
	//            case RemoteController::Button::InterruptsNeeded:
	//                plugins[i]->OnPause();
	//                break;
	//            case RemoteController::Button::IntBlkAllowed:
	//                plugins[i]->OnResume();
	//                break;
	//            default:
	//                plugins[i]->OnRemoteControllerEvent(args);
	//                break;
	//            }*/

	//}

	void SetLeds(CRGB color)
	{
		for (int i = 0; i < controller->size(); i++)
			controller->leds()[i] = color;
	}

	bool updated = false;
	CLEDController* controller = nullptr;
	byte ledsBrightness;

private:
	CRGB* leds;
	byte ledsCount;
	StripPlugin** plugins;
	byte pluginsCount;
	//EventHandler<Strip, RemoteController::EventArgs> remoteControllerEventHandler;
};

extern Strip strip;