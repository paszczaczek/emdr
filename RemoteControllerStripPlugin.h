#pragma once
#include "StripPlugin.h"
#include "RemoteController.h"
#include "Emdr.h"
#include "EventHandler.h"

class RemoteControllerStripPlugin : public StripPlugin
{
public:
	RemoteControllerStripPlugin() 
	{
		remoteController.buttonPressed +=
		new EventHandler<RemoteControllerStripPlugin, RemoteController::EventArgs>
			(this, &RemoteControllerStripPlugin::onRemoteControllerButtonPressed);
	} 

	void onRemoteControllerButtonPressed(RemoteController::EventArgs &args) 
	{
		strip->ledsController->leds()[0] = CRGB::Red;
		strip->ledsController->showLeds();
	}

	virtual void Loop() override
	{ 
	}
};
