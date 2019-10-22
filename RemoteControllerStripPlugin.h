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
public:
	RemoteControllerStripPlugin()
	{
		// subskrybuj eventy od pilota
		remoteController.event += remoteControllerButtonPressedEventHandler.Set(
			this, &RemoteControllerStripPlugin::onRemoteControllerButtonPressed);

		// ustaw timer do migania diode po wcisnieciu guzika na pilocie
		flashTimer.elapsed += flashTimerElapsedEventHandler.Set(
			this, &RemoteControllerStripPlugin::onFlashTimerEvent);
		flashTimer.autoReset = false;

		flashTimer.interval = 250;// 90;
	}

	virtual void Loop() override
	{
		flashTimer.Loop();
		if (state != Plugin::State::Stopped)
			updateLeds();
	}

	virtual void Start() override
	{
		Plugin::Start();
		flashTimer.Start();
	}

private:
	TimerOLD flashTimer;
	EventHandler<RemoteControllerStripPlugin, RemoteController::EventArgs> remoteControllerButtonPressedEventHandler;
	EventHandler<RemoteControllerStripPlugin, TimerOLD::EventArgs> flashTimerElapsedEventHandler;
	
	enum struct LedState : byte { None, Off, Code, CodeUnsupported} ledState = LedState::None;
	bool interruptsNedded = false;

	void onRemoteControllerButtonPressed(RemoteController::EventArgs &args)
	{
		// wcisnieto guzik na pilocie
		switch (args.button)
		{
		case RemoteController::Button::Unknown:
			ledState = LedState::CodeUnsupported;
			flashTimer.Start();
			break;
		case RemoteController::Button::IntBlkDisallowed:
			interruptsNedded = true;
			break;
		case RemoteController::Button::IntBlkAllowed:
			interruptsNedded = false;
			break;
		default:
			ledState = LedState::Code;
			flashTimer.Start();
			break;
		}
	}

	void onFlashTimerEvent(TimerOLD::EventArgs&)
	{
		// czas zgasic diode sygnalizujaca wcisniecie guzika na pilocie
		flashTimer.Stop();
		ledState = LedState::Off;
	}

	void updateLeds()
	{
		// wyznacz kolor jaki ma miec dioda sygnalizujaca wcisniecie guzika na pilocie
		CRGB biColor = CRGB::Black;
		switch (ledState) {
		case LedState::None:
			// zaden guzik na pilocie nie byl wciskany - nie ingerujemy w swiecenie diody
			return;
		case LedState::Off:
			// wcisnieto guzik na pilocie ale czas swiecenia diody minal - wylaczyc diode
			biColor = interruptsNedded ? CRGB::Green : CRGB::Black;
			ledState = LedState::None;
			break;
		case LedState::Code:
			// wcisnieto obslugiwany guzik na pilocie - zapalic diode na czerwono
			biColor = CRGB::Red;
			break;
		case LedState::CodeUnsupported:
			// wcisnieto nieobslugiwany guzik na pilocie - zapalic diode na niebiesko
			biColor = CRGB::Blue;
			break;
		}

		// czy kolor zmienil sie?
		CRGB &biColorCurrent = strip.controller->leds()[0];
		if (biColor.r != biColorCurrent.r ||
			biColor.g != biColorCurrent.g ||
			biColor.b != biColorCurrent.b)
		{
			// tak
			biColorCurrent = biColor;
			// na razie zablokuje, do czasu gdy wyjasni sie czy potrzebny bedzie osobny strip dla remotecontrollera
			// controller->showLeds(2);
			strip.updated = true;
		}
	}
};
