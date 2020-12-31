#pragma once
#include <Arduino.h>
#include <colorutils.h>
#include "Strip.h"
#include "StripPlugin.h"
#include "Timer.h"
#include "Emdr.h"
#include "Debug.h"

class MovingPointStripPlugin : public StripPlugin
{
private:
	// pojemnosc timera poruszajacego punktem
	static const Timer::Capacity movingTimerCapacity = Timer::Capacity::values256; // 8 bitow

	// rozdzielczosc timera poruszajacego punktem
	static const Timer::Interval movingTimerInterval = Timer::Interval::ms16;
	//static const Timer::Interval movingTimerInterval = Timer::Interval::ms1;

	// pojemnosc timera zatrzymujacego poruszajacy sie punkt na krancach tasmy
	static const Timer::Capacity pauseTimerCapacity = Timer::Capacity::values2; // 1 bit

	// rozdzielczosc timera zatrzymujacego poruszajacy sie punkt na krancach tasmy
	static const Timer::Interval pauseTimerResolution = Timer::Interval::s1;

	// czas trwania zabiegu
	static const unsigned long sessionDuration = 60;

	// czas informowanie o koncu zabiegu mierzone w sekundach
	static const unsigned int sessionEndMarkerDuration = 10;

	// number pierwszej diody okna w ktorym porusza sie punkt (first unsigned int)
	unsigned int movingFirstLedNo : 8;

	// numer ostatniej diody okna w ktorym porusza sie punke
	unsigned int movingLastLedNo : 8;

	// numer diody ktora sie scieci
	unsigned int movingLedNo : 8;

	// czas swiecenia punktu wyrazony w ilosci interwalow 1..254
	unsigned int movingTimerCountTo : static_cast<int>(movingTimerCapacity);

	// czas wystartowania timera poruszajacego punktem (next unsigned int)
	unsigned int movingTimerStartedAt : static_cast<int>(movingTimerCapacity);
	
	// kierunek poruszania sie punktu
	unsigned int movingLedDirection : 1;

	// czas wystartowania timera zatrzymujacego punkt na koncach tasmy
	unsigned int pauseTimerStartedAt : static_cast<int>(pauseTimerCapacity);

	// czas zatrzymania punktu na koncach tasmy wyrazony w ilosci interwalow 1..1
	unsigned int pauseTimerCountTo : static_cast<int>(pauseTimerCapacity);

	// czas zatrzymania swiecacego punktu na koncach tasmy
	unsigned int pauseDuration : static_cast<int>(pauseTimerCapacity);

	// kolor poruszajacego sie punktu
	CRGB movingColor = CRGB(CRGB::Orange);
public:
	MovingPointStripPlugin()
	{
		movingTimerStartedAt = Timer::Now(movingTimerInterval, movingTimerCapacity);
		movingFirstLedNo = 0;
		movingLastLedNo = 0;
		movingLedNo = 0;
		movingLedDirection = 0;
		movingTimerCountTo = 0;

		pauseTimerStartedAt = Timer::Now(pauseTimerResolution, pauseTimerCapacity);
		pauseTimerCountTo = 0;
		pauseDuration = 2;
	}

	void Setup(byte movingTimerCountTo = 1, byte movingFirstLedNo = 0, byte movingLastLedNo = 0)
	{
		this->movingTimerCountTo = movingTimerCountTo;
		this->movingFirstLedNo = movingFirstLedNo;
		this->movingLastLedNo = movingLastLedNo;
	}

	// petla zdarzen
	virtual void Loop() override
	{
		// TODO
		//return;

		unsigned long counterStartedAt;
		unsigned long ommittedIntervals;
		
		// przesuwanie swiecacego punktu
		counterStartedAt = movingTimerStartedAt;
		if (pauseTimerCountTo == 0 && Timer::ItsTime(
			movingTimerInterval, movingTimerCapacity,
			&counterStartedAt, movingTimerCountTo,
			&ommittedIntervals, 'm'))
		{
			movingTimerStartedAt = counterStartedAt;
			MovePoint(ommittedIntervals + 1);
		}

		// pauza na krancowych diodach
		counterStartedAt = pauseTimerStartedAt;
		if (Timer::ItsTime(
			pauseTimerResolution, pauseTimerCapacity,
			&counterStartedAt, pauseTimerCountTo,
			&ommittedIntervals))
		{
			pauseTimerStartedAt = counterStartedAt;
			movingTimerStartedAt = Timer::Now(movingTimerInterval, movingTimerCapacity);
			pauseTimerCountTo = 0;
		}
	}

	void MovePoint(unsigned long elapsedIntervals)
	{
		// czy trwa pauza na pierwszej i ostatniej diody
		if (pauseTimerCountTo > 0)
			return;

		while (elapsedIntervals-- > 0)
		{
			// gasimy aktualnie swiecaca sie diode
			strip.controller->leds()[movingLedNo] = CRGB::Black;

			// ruch w kierunku ostaniej diody
			if (movingLedDirection == 0)
				//if (movingLedNo < (unsigned int)strip.controller->size() - 1)
				if (movingLedNo < movingLastLedNo)
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
				//if (movingLedNo > 0)
				if (movingLedNo > movingFirstLedNo)
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

			strip.controller->leds()[movingLedNo] = movingColor;
			strip.updated = true;

			// pauze na pierwszej i ostatniej diodzie
			//if (movingLedNo == 0 && movingLedDirection == 1 ||
			//    movingLedNo == strip.controller->size() - 1 && movingLedDirection == 0)
			if (movingLedNo == movingFirstLedNo && movingLedDirection == 1 ||
				movingLedNo == movingLastLedNo && movingLedDirection == 0)

			{
				// uruchamiany timer odliczajacy pauze
				pauseTimerStartedAt = Timer::Now(pauseTimerResolution, pauseTimerCapacity);
				pauseTimerCountTo = pauseDuration;
				break;
			}
		}
	}

	// wystartowanie lub wznowienie plugina
	void Start() override
	{
		if (state == State::Stopped)
		{
			PRINTLN(F("Start"));
			Plugin::Start();
			movingTimerStartedAt = Timer::Now(movingTimerInterval, movingTimerCapacity);
			
			//PRINT(F("session: ")); PRINT(sessionDuration); PRINTLN(F("s"));
			if (movingLastLedNo == 0)
				movingLastLedNo = (byte)strip.controller->size() - 1;

			strip.controller->leds()[movingFirstLedNo] = movingColor;
			strip.updated = true;

			// TODO
			//FillRainbow();
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
	// zwiekszenie lub zmniejszenie predkosci poruszajacego sie punktu
	void ChangeMovingSpeed(bool increase)
	{
		if (increase)
		{
			if (movingTimerCountTo < 255)
				movingTimerCountTo++;
		}
		else
		{
			if (movingTimerCountTo > 1)
				movingTimerCountTo--;
		}
	}

	// obsluga eventow
	bool Receive(Event::Name eventName) override
	{
		static uint8_t hue = 0;
		switch (eventName)
		{
		case Event::Name::UnknowCode:
			break;
		//case Event::Name::BlockingInterruptsDisallowed:
			//movingCounter.Pause();
			//break;
		//case Event::Name::BlockingInterruptsAllowed:
			//movingCounter.Resume();
			//break;
		case Event::Name::Start:
			Start();
			break;
		case Event::Name::Stop:
			Stop();
			break;
		case Event::Name::Pause:
			Pause();
			break;
		case Event::Name::ChannelPlus:
			Serial.print(F("C+ "));
			Serial.println(hue += 10);
			movingColor.setHue(hue);
			break;
		case Event::Name::ChannelMinus:
			Serial.print(F("C- "));
			Serial.println(hue -= 10);
			movingColor.setHue(hue);
			break;

		}

		return false;
	}

	// ustawia wszystkie ledy w kolory teczy
	void FillRainbow()
	{
		//float deltaHue = 255 / (float)strip.controller->size();
		byte deltaHue = 255 / (movingLastLedNo - movingFirstLedNo + 1);
		fill_rainbow(strip.controller->leds(), strip.controller->size(), 0, deltaHue);
	}

};
