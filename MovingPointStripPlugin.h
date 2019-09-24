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
	CounterUpDownRepeadableSensable movingCounter;
	Timer restTimer;
	unsigned int ledCurrent = 0;
	byte speed = 1; // predkosc poruszania sie punktu wyrazona w liczbie diod na sekunke

public:
	MovingPointStripPlugin()
	{
		movingCounter.interval = 100;
		restTimer.interval = 2000;
	}

	virtual void Loop() override
	{
		// ustawinie ilosci diod
		if (movingCounter.countTo == 0)
			movingCounter.countTo = strip.controller->size() - 1;

		if (restTimer.Elapsed())
			RestTimerElapsed();

		unsigned int counter = 0;
		unsigned int period = 0;
		if (movingCounter.Elapsed(counter, &period))
			MovingCounterElapsed(counter, &period);
	}

	// poruszanie swiecacym punktem
	void MovingCounterElapsed(unsigned int counter, unsigned int* period)
	{
		// na krancach tasmy zrob pauze
		if ((counter == 0 || counter == movingCounter.countTo) && (!period || *period != 0))
		{
			//Serial.println("PAUSE");
			movingCounter.Pause();
			restTimer.Start();
		}

		strip.controller->leds()[ledCurrent] = CRGB::Black;
		strip.controller->leds()[counter] = CRGB::Orange;
		strip.updated = true;

		ledCurrent = counter;
	}

	// pauza na krancach tasmy
	void RestTimerElapsed()
	{
		restTimer.Stop();
		movingCounter.Resume();
		//Serial.println("RESUME");
	}

	virtual void OnStart() override
	{
		Plugin::OnStart();
		SetSpeed(speed = 20);
		movingCounter.Start();
	}

private:
	
	void SetSpeed(byte speed)
	{
		// speed
		movingCounter.interval = (unsigned long)(1 / (float)speed * 1000);
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
		SetSpeed(speed);
	}

	void OnRemoteControllerEvent(RemoteController::EventArgs& args) override
	{
		switch (args.button)
		{
		case RemoteController::Button::IntBlkDisallowed:
			//movingTimer.Stop();
			break;
		case RemoteController::Button::IntBlkAllowed:
			//movingTimer.Start();
			break;
		case RemoteController::Button::CHANEL_PLUS:
			changeSpeed(true);
			break;
		case RemoteController::Button::CHANEL_MINUS:
			changeSpeed(false);
			break;
		case RemoteController::Button::PLAY:
			//movingTimer.Start();
			break;
		case RemoteController::Button::PAUSE:
			//movingTimer.Stop();
			break;
		default:
			break;
		}
	}
};
