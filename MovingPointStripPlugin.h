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
	// licznik poruszajacy swiecacym punktem
	CounterPeriodic movingCounter;

	// timer zatrzymujacy swiecacy punkt na koncach tasmy
	Timer restTimer;

	// timer odmierzajacy czas zabiegu
	Timer sessionTimer;

	// nummer pierwszej swiecacej diody; numer ostatniej diody jest w movingCounter.countTo + ledFirst
	byte ledFirst = 0;

	// numer aktualnie swiecacgo punktu
	byte ledCurrent = 0;

	// predkosc poruszania sie swiecacego punktu wyrazona w liczbie diod na sekunde
	byte movingSpeed = 200;

	// kolor poruszajacego sie punktu
	CRGB movingColor = CRGB(CRGB::Orange);

	// czas zatrzymania swiecacego punktu na koncach tasmy mierzony w sekunach
#define restDuration 2

// czas trwania zabiegu i czas informowanie o koncu zabiegu mierzone w sekundach
#define sessionDuration    (unsigned long)60
#define sessionEndDuration (unsigned long)10

public:
	MovingPointStripPlugin()
	{
		movingCounter.interval = 5;
		movingCounter.countTo = 30;
		restTimer.interval = restDuration * 1000;
		sessionTimer.interval = sessionDuration * 1000;
	}

	// petla zdarzen
	virtual void Loop() override
	{
		// poruszanie swiecacym punktem
		unsigned int counter = 0;
		unsigned int period = 0;
		if (movingCounter.ItsTime(
			CounterPeriodic::Mode::UpDown,
			CounterPeriodic::Options::CatchMinMax,
			counter,
			&period))
			MovingCounterItsTime(counter, &period);

		// pauza na krancach tasmy
		if (restTimer.ItsTime(Timer::Mode::SingleShot))
			RestTimerItsTime();

		// sygnalizacja miniecia czasu zabiegu
		if (sessionTimer.ItsTime(Timer::Mode::MultiShot))
			SessionTimerItsTime();
	}

	// poruszanie swiecacym punktem
	void MovingCounterItsTime(unsigned int counter, unsigned int* period)
	{
		// na krancach tasmy zrob pauze
		if ((counter == 1 && *period > 1) ||
			counter == movingCounter.countTo)
		{
			//Serial.println(F("pause"));
			movingCounter.Pause();
			restTimer.Start();
		}

		// gasimy aktualnie swiecaca sie diode
		strip.controller->leds()[ledCurrent] = CRGB::Black;

		// i zapalamy nastepna 
		ledCurrent = counter - 1 + ledFirst;
		strip.controller->leds()[ledCurrent] = movingColor;

		strip.updated = true;
	}

	// koniec pauzy na krancach tasmy
	void RestTimerItsTime()
	{
		//Serial.println(F("resume"));
		restTimer.Stop();
		movingCounter.Resume();
	}

	// mierzenie czasu zabiegu
	void SessionTimerItsTime()
	{
		if (sessionTimer.interval == sessionEndDuration * 1000)
		{
			// minal czas sygnalizacji konca zabiegu, przywracamy normaly kolor
			//Serial.print(F("session cont: ")); Serial.println(millis());
			movingColor = -movingColor;
			strip.controller->leds()[ledCurrent] = movingColor;
			strip.updated = true;
			sessionTimer.interval = (sessionDuration - sessionEndDuration) * 1000;
			return;
		}
		else
		{
			// minal czas zabiegu, sugnalizujemy to chwilowa zmiana koloru poruszajacego sie punktu
			Serial.print(F("session end: ")); Serial.println(millis());
			movingColor = -movingColor;
			strip.controller->leds()[ledCurrent] = movingColor;
			strip.updated = true;
			sessionTimer.interval = sessionEndDuration * 1000;
			return;
		}
	}

	void Start() override
	{
		switch (state)
		{
		case State::Started:
			break;
		case State::Stopped:
		case State::Paused:
			Plugin::Start();
			SetMovingSpeed(movingSpeed);
			movingCounter.Start();
			sessionTimer.Start();
			PRINTLN(F("Start"));
			//PRINT(F("session: ")); PRINT(sessionDuration); PRINTLN(F("s"));
		}
	}

	void Stop() override
	{
		switch (state)
		{
		case Plugin::State::Started:
		case Plugin::State::Stopped:
		case Plugin::State::Paused:
			Plugin::Stop();
			movingCounter.Stop();
			restTimer.Stop();
			sessionTimer.Stop();
			PRINTLN(F("Stop"));
		}
	}

	void Pause() override
	{
		switch (state)
		{
		case State::Started:
			Plugin::Pause();
			movingCounter.Pause();
			restTimer.Stop();
			sessionTimer.Stop();
			PRINTLN(F("Pause"));
			break;
		case State::Stopped:
			break;
		case State::Paused:
			Plugin::Resume();
			movingCounter.Resume();
			restTimer.Start();
			sessionTimer.Start();
			PRINTLN(F("Resume"));
			break;
		}
	}

private:
	// ustawienie predkosci poruszajacego sie punktu w diodach na sekunde
	void SetMovingSpeed(byte speed)
	{
		// speed
		movingCounter.interval = (unsigned long)(1 / (float)speed * 1000);
		PRINT(F("speed: ")); PRINT(speed); PRINTLN(F("d/s"));
	}

	void ChangeMovingSpeed(bool increase)
	{
		if (increase)
		{
			if (movingSpeed < 180)
				movingSpeed++;
		}
		else
		{
			if (movingSpeed > 1)
				movingSpeed--;
		}
		SetMovingSpeed(movingSpeed);
	}

	bool Receive(Event::Name eventName) override
	{
		switch (eventName)
		{
		case Event::Name::UnknowCode:
			break;
		case Event::Name::BlockingInterruptsDisallowed:
			movingCounter.Pause();
			break;
		case Event::Name::BlockingInterruptsAllowed:
			movingCounter.Resume();
			break;
		case Event::Name::Start:
			Start();
			break;
		case Event::Name::Stop:
			Stop();
			break;
		case Event::Name::Pause:
			Pause();
			break;
		case Event::Name::CHANEL_PLUS:
			ChangeMovingSpeed(true);
			break;
		case Event::Name::CHANEL_MINUS:
			ChangeMovingSpeed(false);
			break;
		}

		return false;
	}
};
