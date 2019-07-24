#pragma once
#include "StripPlugin.h"
#include "Timer.h"
#include "RemoteController.h"

class MovingPointStripPlugin : public StripPlugin
{
public:
	MovingPointStripPlugin()
	{
		movingTimer.elapsed += new EventHandler<MovingPointStripPlugin, Timer::EventArgs>
			(this, &MovingPointStripPlugin::onMovingTimerElapsed);
		movingTimer.interval = 1000;
		movingTimer.Start();
	}

	~MovingPointStripPlugin()
	{
		movingTimer.elapsed -= new EventHandler<MovingPointStripPlugin, Timer::EventArgs>
			(this, &MovingPointStripPlugin::onMovingTimerElapsed);
		strip->remoteController.buttonPressed -= new EventHandler<MovingPointStripPlugin, RemoteController::EventArgs>
			(this, &MovingPointStripPlugin::OnRemoteControllerButtonPressed);
	}

	virtual void OnSetStrip(Strip *strip) override
	{
		strip->remoteController.buttonPressed += new EventHandler<MovingPointStripPlugin, RemoteController::EventArgs>
			(this, &MovingPointStripPlugin::OnRemoteControllerButtonPressed);
	}

	virtual void Loop() override
	{
		movingTimer.Loop();
	}

private:
	Timer movingTimer;
	int ledCurrent = 0;
	enum MovingDirection { RIGTH, LEFT } movingDirection = RIGTH;;

	void onMovingTimerElapsed(Timer::EventArgs& args)
	{
		auto ledNext = ledCurrent;

		if (movingDirection == MovingDirection::RIGTH)
			if (ledCurrent < strip->controller->size() - (int)args.elapsedIntervals)
				ledNext += args.elapsedIntervals;
			else
				movingDirection = MovingDirection::LEFT;
		else
			if (ledCurrent > (int)args.elapsedIntervals - 1)
				ledNext -= args.elapsedIntervals;
			else
				movingDirection = MovingDirection::RIGTH;

		if (ledNext == ledCurrent)
			return;

		strip->controller->leds()[ledCurrent] = CRGB::Black;
		strip->controller->leds()[ledNext] = CRGB::Orange;
		strip->updated = true;
		//Serial.println("MovingPointStripPlugin::onMovingTimerElapsed");

		ledCurrent = ledNext;
	}

	void OnRemoteControllerButtonPressed(RemoteController::EventArgs &args)
	{
		switch (args.button)
		{
		case RemoteController::Button::CHANEL_PLUS:
			if (movingTimer.interval > 20)
				movingTimer.interval /= 2;
			break;
		case RemoteController::Button::CHANEL_MINUS:
			movingTimer.interval *= 2;
			break;
		case RemoteController::Button::PLAY:
			movingTimer.Start();
			break;
		case RemoteController::Button::PAUSE:
			movingTimer.Stop();
			break;
		}
	}
};
