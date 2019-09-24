#pragma once
#include <Arduino.h>
#include "Strip.h"
#include "StripPlugin.h"
#include "Timer.h"
//#include "RemoteController.h"
#include "Emdr.h"

class MovingPointStripPlugin : public StripPlugin
{
private:
	CounterUpDownRepeadable movingCounter;
	Timer restTimer;

public:
	MovingPointStripPlugin()
	{
		//elapsedEventHandler.Set(this, &MovingPointStripPlugin::onMovingTimerElapsed);
		movingTimer.interval = 10000;
		
		movingCounter.interval = 20;
		movingCounter.Start();
		restTimer.interval = 3000;
	}

	virtual void Loop() override
	{
		/*unsigned int intevals = 0;
		if (movingTimer.Elapsed(intevals))
			MovingTimerElapsed(intevals);*/
		if (movingCounter.countTo == 0)
			movingCounter.countTo = strip.controller->size() - 1;

		if (restTimer.Elapsed())
			RestTimerElapsed();

		unsigned int counter = 0;
		CounterUpDownRepeadable::Info info;
		unsigned int ommittedMinMaxConters;
		if (movingCounter.Elapsed(counter, &info, &ommittedMinMaxConters))
			MovingCounterElapsed(counter, info, ommittedMinMaxConters);
	}

	void RestTimerElapsed()
	{
		restTimer.Stop();
		movingCounter.Resume();
		Serial.println("RESUME");
	}

	void MovingCounterElapsed(unsigned int ledNext, CounterUpDownRepeadable::Info info, unsigned int ommittedMinMaxConters)
	{
		switch (info)
		{
		case CounterUpDownRepeadable::Info::MinOccurred:
		case CounterUpDownRepeadable::Info::MaxOccurred:
			movingCounter.Pause();
			restTimer.Start();
			Serial.println("MIN/MAX");
			break;
		case CounterUpDownRepeadable::Info::MinOmmited:
			movingCounter.Pause();
			movingCounter.Decrease(ommittedMinMaxConters);
			restTimer.Start();
			Serial.println("MIN OMMITTED");
			ledNext = 0;
			break;
		case CounterUpDownRepeadable::Info::MaxOmmited:
			movingCounter.Pause();
			movingCounter.Decrease(ommittedMinMaxConters);
			restTimer.Start();
			Serial.println("MAX OMMITTED");
			ledNext = movingCounter.countTo;
			break;
		default:
			break;
		}

		strip.controller->leds()[ledCurrent] = CRGB::Black;
		strip.controller->leds()[ledNext] = CRGB::Orange;
		strip.updated = true;

		ledCurrent = ledNext;

		/*
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
		*/
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
	unsigned int ledCurrent = 0;
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

	/*void onMovingTimerElapsed(TimerOLD::EventArgs& args)
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
	}*/

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
