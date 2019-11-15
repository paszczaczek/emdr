#pragma once
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <inttypes.h>
#include "Strip.h"
#include "StripPlugin.h"
#include "RemoteController.h"
#include "Emdr.h"
#include "EventHandler.h"
#include "Timer.h"

class RemoteControllerStripPlugin : public StripPlugin
{
private:
	Timer flashTimer;

public:
	RemoteControllerStripPlugin()
	{
		receivingLedState = ReceivingLedState::None;
		blockingInterruptsLedState = BlockingInterruptsLedState::None;
		flashTimer.interval = 90;
		Start();
	}

	virtual void Loop() override
	{
		if (flashTimer.ItsTime(Timer::Mode::SingleShot))
		{
			FlashTimerItsTime();
		}
		else if (strip.updated)
		{
			UpdateReceivingLeds();
			UpdateBlockingInterruptsLeds();
		}
	}

private:
	enum struct ReceivingLedState : byte { None, TurnOn, TurnOnUnsupported, TurnOff };
	enum struct BlockingInterruptsLedState : byte { None, TurnOn, TurnOff };

	// stan diody sygnalizujacej wcisniecie guzika na pilocie
	ReceivingLedState receivingLedState : 2;

	// stan diody sygnalizujacej zakaz blokowania przerwan
	BlockingInterruptsLedState blockingInterruptsLedState : 2;

	void FlashTimerItsTime()
	{
		// czas zgasic diode sygnalizujaca wcisniecie guzika na pilocie
		receivingLedState = ReceivingLedState::TurnOff;
		UpdateReceivingLeds();
		//ledUpdateNeeded = true;
	}

	bool Receive(Event::Name eventName) override
	{
		// wcisnieto guzik na pilocie
		switch (eventName)
		{
		case Event::Name::BlockingInterruptsDisallowed:
			blockingInterruptsLedState = BlockingInterruptsLedState::TurnOn;
			UpdateBlockingInterruptsLeds();
			break;
		case Event::Name::BlockingInterruptsAllowed:
			blockingInterruptsLedState = BlockingInterruptsLedState::TurnOff;
			UpdateBlockingInterruptsLeds();
			break;
		case Event::Name::UnknowCode:
			receivingLedState = ReceivingLedState::TurnOnUnsupported;
			flashTimer.Start();
			UpdateReceivingLeds();
			break;
		default:
			receivingLedState = ReceivingLedState::TurnOn;
			flashTimer.Start();
			UpdateReceivingLeds();
			break;
		}

		return false;
	}

	// altualizacja koloru diody sygnalizujacej wcisniecie guzika na pilocie
	void UpdateReceivingLeds()
	{
		CRGB ledColorNew = CRGB::Black;
		switch (receivingLedState) {
		case ReceivingLedState::None:
			// zaden guzik na pilocie nie byl wciskany - nie ingerujemy w swiecenie diody
			return;
		case ReceivingLedState::TurnOn:
			// wcisnieto obslugiwany guzik na pilocie - zapalic diode na czerwono
			ledColorNew = CRGB::Red;
			break;
		case ReceivingLedState::TurnOnUnsupported:
			// wcisnieto nieobslugiwany guzik na pilocie - zapalic diode na niebiesko
			ledColorNew = CRGB::Violet;
			break;
		case ReceivingLedState::TurnOff:
			// wczesniej wcisnieto guzik na pilocie - nadszedl czas zgaszenia go
			receivingLedState = ReceivingLedState::None;
			ledColorNew = CRGB::Black;
			break;
		}

		// czy kolor zmienil sie?
		CRGB& ledColorCurrent = strip.controller->leds()[0];
		if (ledColorNew.r != ledColorCurrent.r ||
			ledColorNew.g != ledColorCurrent.g ||
			ledColorNew.b != ledColorCurrent.b)
		{
			// tak
			ledColorCurrent = ledColorNew;
			strip.updated = true;
		}
	}

	// altualizacja koloru diody sygnalizujacej zakaz blokowania przerwan
	void UpdateBlockingInterruptsLeds()
	{
		CRGB ledColorNew = CRGB::Black;
		switch (blockingInterruptsLedState) {
		case BlockingInterruptsLedState::None:
			// zaden guzik na pilocie nie byl wciskany - nie ingerujemy w swiecenie diody
			return;
		case BlockingInterruptsLedState::TurnOn:
			// wcisnieto obslugiwany guzik na pilocie - zapalic diode na czerwono
			ledColorNew = CRGB::Green;
			break;
		case BlockingInterruptsLedState::TurnOff:
			// wcisnieto nieobslugiwany guzik na pilocie - zapalic diode na niebiesko
			blockingInterruptsLedState = BlockingInterruptsLedState::None;
			ledColorNew = CRGB::Black;
			break;
		}

		// czy kolor zmienil sie?
		CRGB& ledColorCurrent = strip.controller->leds()[1];
		if (ledColorNew.r != ledColorCurrent.r ||
			ledColorNew.g != ledColorCurrent.g ||
			ledColorNew.b != ledColorCurrent.b)
		{
			// tak
			ledColorCurrent = ledColorNew;
			strip.updated = true;
		}
	}
};
