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
	CounterPeriodic testCounter;
	// licznik poruszajacy swiecacym punktem
	//CounterUpDownPeriodicDetectingMinMax movingCounter;
	CounterPeriodic movingCounter;

	// timer zatrzymujacy swiecacy punkt na koncach tasmy
	Timer restTimer;

	// timer odmierzajacy czas zabiegu
	Timer sessionTimer;

	// number aktualnie swiecacgo punktu
	unsigned int ledCurrent = 0;

	// predkosc poruszania sie swiecacego punktu wyrazona w liczbie diod na sekunke
	byte speed = 1;

	// kolor poruszajacego sie punktu
	CRGB movingColor = CRGB(CRGB::Orange);

public:
	MovingPointStripPlugin()
	{
		//movingCounter.interval = 100;
		restTimer.interval = 2000;
		//testCounter.interval = 1000;
	}

	virtual void Loop() override
	{
		// ustawinie ilosci diod
		if (movingCounter.countTo == 0)
			movingCounter.countTo = strip.controller->size();// -1;

		// pauza na krancach tasmy
		if (restTimer.ItsTime(Timer::Mode::SingleShot))
			RestTimerElapsed();

		// poruszanie swiecacym punktem
		unsigned int counter = 0;
		unsigned int period = 0;
		if (movingCounter.ItsTime(
			CounterPeriodic::Mode::UpDown,
			CounterPeriodic::Options::CatchMinMax,
			counter,
			&period))
			MovingCounterItsTime(counter, &period);

		// sygnalizacja miniecia czasu zabiegu
		if (sessionTimer.ItsTime(Timer::Mode::MultiShot))
			SessionTimerItsTime();

		if (testCounter.ItsTime(
			CounterPeriodic::Mode::UpDown,
			CounterPeriodic::Options::WithZero | CounterPeriodic::Options::CatchMinMax,
			counter,
			&period))
		{
			Serial.print("Test: ");
			Serial.print(counter);
			Serial.print(" / ");
			Serial.println(period);
		}

		// mierzenie czasu zabiegu
		/*unsigned int sessionCounterValue = 0;
		unsigned int sessionCounterPeriod = 0;
		if (sessionCounter.Elapsed(sessionCounterValue, &sessionCounterPeriod))
			SessionCounterElapsed(sessionCounterValue, sessionCounterPeriod);*/
	}

	// poruszanie swiecacym punktem
	void MovingCounterItsTime(unsigned int counter, unsigned int* period)
	{
		//Serial.print(counter);
		//Serial.print(", ");
		//Serial.println(*period);

		if (counter == 0)
			return;
		// na krancach tasmy zrob pauze
		if ((counter == 1 && *period > 1) ||
			counter == movingCounter.countTo)
		{
			//Serial.println("PAUSE");
			movingCounter.Pause();
			restTimer.Start();
		}

		strip.controller->leds()[ledCurrent] = CRGB::Black;
		strip.controller->leds()[counter - 1] = movingColor;
		strip.updated = true;

		ledCurrent = counter - 1;
	}

	// pauza na krancach tasmy
	void RestTimerElapsed()
	{
		restTimer.Stop();
		movingCounter.Resume();
		//Serial.println("RESUME");
	}

	// mierzenie czasu zabiegu
	void SessionTimerItsTime(/*unsigned int counter, unsigned int period*/)
	{
		// czy minal czas zabiegu?
		// if (counter == sessionTimer.countTo)
		if (sessionTimer.interval == 10 * 1000)
		{

			// tak - sugnalizujemy to chwilowa zmiana koloru poruszajacego sie punktu
			Serial.print(F("Session blue: ")); Serial.println(millis());
			movingColor = -movingColor;
			strip.controller->leds()[ledCurrent] = movingColor;
			strip.updated = true;
			sessionTimer.interval = 5 * 1000;
			return;
		}
		else
		{
			// czy minal czas sygnalizacji konca zabiegu?
			//if (counter == 0 && period > 1)
			//{
				// tak - przywracamy normalny kolor poruszajacego sie punktu
				//Serial.print(counter);  Serial.println(F(": Session orange"));
			Serial.print(F("Session orange: ")); Serial.println(millis());
			movingColor = -movingColor;
			strip.controller->leds()[ledCurrent] = movingColor;
			strip.updated = true;
			sessionTimer.interval = 10 * 1000;
			return;
		}

		//Serial.println(counter);

		/*
		switch (counter)
		{
		case 0:
			// czas zabiegu jeszcze nie minal
			break;
		case 1:
			// minal czas zabiegu - zaczynamy migac swiecacym punktem
			Serial.print(F("Session finished: ")); Serial.println(period + 1);
			sessionFinished = 1;
			strip.controller->leds()[ledCurrent] = CRGB::Red;
			strip.updated = true;
			break;
		case 2:
			// konczymy miganie, bedzie kolejna runda zabiegu
			sessionFinished = 0;
			strip.controller->leds()[ledCurrent] = CRGB::Orange;
			strip.updated = true;
			break;
		}
		*/
	}

	virtual void OnStart() override
	{
		Plugin::OnStart();
		SetSpeed(speed = 20);
		movingCounter.Start();
		movingCounter.interval = 5;
		sessionTimer.interval = 10 * 1000;
		//sessionTimer.countTo = 5;
		//sessionCounter.Start();
		sessionTimer.interval = 10 * 1000;
		sessionTimer.Start();

		testCounter.interval = 1000;
		testCounter.countTo = 4;
		//testCounter.Start();
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
