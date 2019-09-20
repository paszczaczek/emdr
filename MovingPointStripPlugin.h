#pragma once
#include "Strip.h"
#include "StripPlugin.h"
#include "Timer.h"
//#include "RemoteController.h"
#include "Emdr.h"

class MovingPointStripPlugin : public StripPlugin
{
public:
	MovingPointStripPlugin()
	{
		//elapsedEventHandler.Set(this, &MovingPointStripPlugin::onMovingTimerElapsed);
		movingTimer.interval = 10000;
		//movingCounter.to = 10;
		movingCounter.interval = 1000;
		movingCounter.countTo = 5;
		movingCounter.Start();
	}

	virtual void Loop() override
	{
		unsigned int intevals = 0;
		if (movingTimer.Elapsed(intevals))
			MovingTimerElapsed(intevals);
		unsigned int counter = 0;
		if (movingCounter.Elapsed(counter))
			;// Serial.println(counter);
	}

	void MovingTimerElapsed(unsigned int elapsedIntervals)
	{
		long ledNext = ledCurrent;

		if (movingDirection == MovingDirection::RIGTH)
			if (ledCurrent < strip.controller->size() - (int)elapsedIntervals)
				ledNext += elapsedIntervals;
			else
				movingDirection = MovingDirection::LEFT;
		else if (ledCurrent > (int)elapsedIntervals - 1)
			ledNext -= elapsedIntervals;
		else
			movingDirection = MovingDirection::RIGTH;

		if (ledNext < 0 || ledNext >= strip.controller->size())
			Serial.println(F("*MovingPointStrip::MovingTimerElapsed: ledNext bad!"));

		if (ledNext == ledCurrent)
			return;

		strip.controller->leds()[ledCurrent] = CRGB::Black;
		strip.controller->leds()[ledNext] = CRGB::Orange;
		strip.updated = true;

		ledCurrent = ledNext;
	}

	virtual void OnStart() override
	{
		Plugin::OnStart();
		//setSpeed(speed = 70);
		setSpeed(speed = 10);
		movingTimer.Start();
	}

	virtual void OnPause() override
	{
		PRINTLN(F("MovingPointStripPlugin::OnPause"));
		Plugin::OnPause();
		movingTimer.Stop();
	}

	virtual void OnResume() override
	{
		PRINTLN(F("MovingPointStripPlugin::OnResume"));
		Plugin::OnResume();
		movingTimer.Start();
	}

private:
	TimerRepeatable movingTimer;
	CounterUpToDown movingCounter;
	int ledCurrent = 0;
	byte speed = 1; // predkosc poruszania sie punktu wyrazona w liczbie diod na sekunke
	enum MovingDirection { RIGTH, LEFT } movingDirection = RIGTH;
	//EventHandler<MovingPointStripPlugin, Timer::EventArgs> elapsedEventHandler;

	void setSpeed(byte speed)
	{
		// speed
		movingTimer.interval = (unsigned long)(1 / (float)speed * 1000);
		PRINT(F("Speed: ")); PRINT(speed); PRINTLN(F("d/s"));
	}

	void changeSpeed(bool increase)
	{
		if (increase)
		{
			if (speed < 180)
				speed++;
		}
		else
		{
			if (speed > 1)
				speed--;
		}
		setSpeed(speed);
	}

	void onMovingTimerElapsed(Timer::EventArgs& args)
	{
		auto ledNext = ledCurrent;

		Serial.println(args.elapsedIntervals);
		Serial.println(args.elapsedIntervalsFromStart);
		if (movingDirection == MovingDirection::RIGTH)
			if (ledCurrent < strip.controller->size() - (int)args.elapsedIntervals)
				ledNext += args.elapsedIntervals;
			else
				movingDirection = MovingDirection::LEFT;
		else if (ledCurrent > (int)args.elapsedIntervals - 1)
			ledNext -= args.elapsedIntervals;
		else
			movingDirection = MovingDirection::RIGTH;

		if (ledNext == ledCurrent)
			return;

		strip.controller->leds()[ledCurrent] = CRGB::Black;
		strip.controller->leds()[ledNext] = CRGB::Orange;
		strip.updated = true;

		ledCurrent = ledNext;
	}

	void OnRemoteControllerEvent(RemoteController::EventArgs& args) override
	{
		switch (args.button)
		{
		case RemoteController::Button::IntBlkDisallowed:
			movingTimer.Stop();
			break;
		case RemoteController::Button::IntBlkAllowed:
			movingTimer.Start();
			break;
		case RemoteController::Button::CHANEL_PLUS:
			changeSpeed(true);
			break;
		case RemoteController::Button::CHANEL_MINUS:
			changeSpeed(false);
			break;
		case RemoteController::Button::PLAY:
			movingTimer.Start();
			break;
		case RemoteController::Button::PAUSE:
			movingTimer.Stop();
			break;
		default:
			break;
		}
	}
};
