#pragma once
#include "StripPlugin.h"
#include "Timer.h"
#include "RemoteController.h"

class PowerStipPlugin : public StripPlugin
{
public:
	PowerStipPlugin()
	{
		timer.elapsed += new EventHandler<PowerStipPlugin, Timer::EventArgs>
			(this, &PowerStipPlugin::onTimerElapsed);
		timer.interval = 2000;
	}

	~PowerStipPlugin()
	{
		timer.elapsed -= new EventHandler<PowerStipPlugin, Timer::EventArgs>
			(this, &PowerStipPlugin::onTimerElapsed);
		strip->remoteController.buttonPressed -= new EventHandler<PowerStipPlugin, RemoteController::EventArgs>
			(this, &PowerStipPlugin::OnRemoteControllerButtonPressed);
	}

	virtual void OnSetStrip(Strip *strip) override
	{
		strip->remoteController.buttonPressed += new EventHandler<PowerStipPlugin, RemoteController::EventArgs>
			(this, &PowerStipPlugin::OnRemoteControllerButtonPressed);
		Plugin::OnStart();
		state = PowerOn;
		UpdateLeds();
		timer.Start();
	}

	virtual void Loop() override
	{
		timer.Loop();
		UpdateLeds();
	}

private:
	Timer timer;
	enum State { PowerOn, PowerOff, Waiting, Ready, Inactive } state = Inactive;

	void onTimerElapsed(Timer::EventArgs& args)
	{
		state = Ready;
		strip->controller->clearLedData();
		UpdateLeds();
		timer.Stop();
	}

	void UpdateLeds()
	{
		switch (state)
		{
		case PowerOn:
			for (int i = 0; i < strip->controller->size(); i++)
				strip->controller->leds()[i] = CRGB::White;
			strip->updated = true;
			state = Waiting;
			break;
		case PowerOff:
			for (int i = 0; i < strip->controller->size(); i++)
				strip->controller->leds()[i] = CRGB::Blue;
			strip->updated = true;
			state = Waiting;
			break;
		case Waiting:
			break;
		case Ready:
			for (int i = 0; i < strip->controller->size(); i++)
				strip->controller->leds()[i] = CRGB::Black;
			strip->updated = true;
			strip->StartAllPlugins();
			state = Inactive;
			break;
		case Inactive:
			break;
		}
	}

	void OnRemoteControllerButtonPressed(RemoteController::EventArgs &args)
	{
		//switch (args.button)
		//{
		//}
	}
};
