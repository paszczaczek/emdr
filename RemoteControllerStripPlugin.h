#pragma once
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
		remoteController.buttonPressed +=
		new EventHandler<RemoteControllerStripPlugin, RemoteController::EventArgs>
			(this, &RemoteControllerStripPlugin::onRemoteControllerButtonPressed);

		flashTimer.elapsed += new EventHandler<RemoteControllerStripPlugin, Timer::EventArgs>
			(this, &RemoteControllerStripPlugin::onTimerFlashEvent);
		flashTimer.interval = 100;
		flashTimer.autoReset = false;
	}

	virtual void Loop() override
	{
		flashTimer.Loop();
	}

private:
	Timer flashTimer;

	void onRemoteControllerButtonPressed(RemoteController::EventArgs &args) 
	{
		strip->ledsController->leds()[0] 
			= args.button == RemoteController::Button::UNSUPPORTED
			? CRGB::Blue
			: CRGB::Red;
		strip->ledsController->showLeds();
		flashTimer.Start();
	}

	void onTimerFlashEvent(Timer::EventArgs& args)
	{
		strip->ledsController->leds()[0] = CRGB::Black;
		strip->ledsController->showLeds();
	}
};
