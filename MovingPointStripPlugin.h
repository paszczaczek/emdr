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
	// predkosc poruszania sie swiecacego punktu wyrazona w liczbie diod na sekunde
	byte movingSpeed = 10; // 200;

	// kolor poruszajacego sie punktu
	CRGB movingColor = CRGB(CRGB::Orange);

	// czas trwania zabiegu
	static const unsigned long sessionDuration = 60;

	// czas informowanie o koncu zabiegu mierzone w sekundach
	static const unsigned int sessionEndMarkerDuration = 10;

	static const Timer2::Interval movingTimerInterval = Timer2::Interval::ms32;
	static const Timer2::Capacity movingTimerCapacity = Timer2::Capacity::bit1;

	static const Timer2::Interval pauseTimerInterval = Timer2::Interval::ms512;
	static const Timer2::Capacity pauseTimerCapacity = Timer2::Capacity::bits4;

	unsigned int movingTimerStartedAt : static_cast<int>(movingTimerCapacity);
	unsigned int movingLedNo : 8;
	unsigned int movingLedDirection : 1;

	unsigned int pauseTimerStartedAt : static_cast<int>(pauseTimerCapacity);
	unsigned int pauseTimerCountTo : static_cast<int>(pauseTimerCapacity);
	
	// czas zatrzymania swiecacego punktu na koncach tasmy
	unsigned int pauseDuration : static_cast<int>(pauseTimerCapacity);

public:
	MovingPointStripPlugin()
	{
		//restTimer.interval = restDuration * 1000;
		//sessionTimer.interval = sessionDuration * 1000;

		movingTimerStartedAt = Timer2::Now(movingTimerInterval, movingTimerCapacity);
		movingLedNo = 0;
		movingLedDirection = 0;

		pauseTimerStartedAt = Timer2::Now(pauseTimerInterval, pauseTimerCapacity);
		pauseTimerCountTo = 0;
		pauseDuration = 2;
	}

	// petla zdarzen
	virtual void Loop() override
	{
		unsigned long counterStartedAt;
		
		// przesuwanie swiecacego punktu
		counterStartedAt = movingTimerStartedAt;
		if (pauseTimerCountTo == 0 && Timer2::ItsTime(
			movingTimerInterval, movingTimerCapacity,
			&counterStartedAt, 1, 'm'))
		{
			movingTimerStartedAt = counterStartedAt;
			MovePoint();
		}

		// pauza na krancowych diodach
		counterStartedAt = pauseTimerStartedAt;
		if (Timer2::ItsTime(
			pauseTimerInterval, pauseTimerCapacity,
			&counterStartedAt, pauseTimerCountTo, 'p'))
		{
			pauseTimerStartedAt = counterStartedAt;
			movingTimerStartedAt = counterStartedAt;
			pauseTimerCountTo = 0;
		}
	}

	void MovePoint()
	{
		// czy trwa pauza na pierwszej i ostatniej diody
		if (pauseTimerCountTo > 0)
			return;

		// gasimy aktualnie swiecaca sie diode
		strip.controller->leds()[movingLedNo] = CRGB::Black;

		// ruch w kierunku ostaniej diody
		if (movingLedDirection == 0)
			if (movingLedNo < strip.controller->size() - 1)
			{
				// nastepna dioda
				movingLedNo++;
			}
			else
			{
				// ostatnia dioda - zmiana kierunku
				movingLedDirection = 1;
				movingLedNo--;
			}
		// ruch w kierunku pierwszej diody
		else
			if (movingLedNo > 0)
			{
				// poprzednia dioda
				movingLedNo--;
			}
			else
			{
				// pierwsza dioda - zmiana kierunku
				movingLedDirection = 0;
				movingLedNo++;
			}

		// pauze na pierwszej i ostatniej diodzie
		if (movingLedNo == 0 && movingLedDirection == 1 ||
			movingLedNo == strip.controller->size() - 1 && movingLedDirection == 0)
		{
			pauseTimerStartedAt = Timer2::Now(pauseTimerInterval, pauseTimerCapacity);
			pauseTimerCountTo = pauseDuration;
		}

		strip.controller->leds()[movingLedNo] = movingColor;
		strip.updated = true;
	}

	// wystartowanie lub wznowienie plugina
	void Start() override
	{
		if (state == State::Stopped)
		{
			Plugin::Start();
			SetMovingSpeed(movingSpeed);
			//movingCounter.Start();
			//sessionTimer.Start();
			PRINTLN(F("Start"));
			//PRINT(F("session: ")); PRINT(sessionDuration); PRINTLN(F("s"));
		}
		else if (state == State::Paused)
		{
			Plugin::Start();
			//movingCounter.Resume();
			//restTimer.Start();
			//sessionTimer.Start();
			PRINTLN(F("Resume"));
		}
	}

	// zatrzymanie plagina
	void Stop() override
	{
		if (state != State::Stopped)
		{
			Plugin::Stop();
			//movingCounter.Stop();
			//restTimer.Stop();
			//sessionTimer.Stop();
			PRINTLN(F("Stop"));
		}
	}

	// zapauzowanie plagina
	void Pause() override
	{
		if (state == State::Started)
		{
			Plugin::Pause();
			//movingCounter.Pause();
			//restTimer.Stop();
			//sessionTimer.Stop();
			PRINTLN(F("Pause"));
		}
	}

private:
	// ustawienie predkosci poruszajacego sie punktu w diodach na sekunde
	void SetMovingSpeed(byte speed)
	{
		// speed
		//movingCounter.interval = (unsigned long)(1 / (float)speed * 1000);
		PRINT(F("speed: ")); PRINT(speed); PRINTLN(F("d/s"));
	}

	// zwiêkszenie lub zmniejszenie predkosci poruszajacego sie punktu
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

	// obsluga eventow
	bool Receive(Event::Name eventName) override
	{
		switch (eventName)
		{
		case Event::Name::UnknowCode:
			break;
		case Event::Name::BlockingInterruptsDisallowed:
			//movingCounter.Pause();
			break;
		case Event::Name::BlockingInterruptsAllowed:
			//movingCounter.Resume();
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
