#pragma once
#include <Arduino.h>
#include <colorutils.h>
#include <math.h>
#include "Strip.h"
#include "StripPlugin.h"
#include "Timer.h"
#include "Emdr.h"
#include "Debug.h"
#include "lcd.h"
#include "Storage.h"

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
	bool Receive(Event::Name eventReceived) override
	{
		constexpr byte eoeFun = static_cast<byte>(Storage::MovingPointStripPlugin_Fun::EOE);
		constexpr byte firstFun = 0;
		byte fun = static_cast<byte>(storage.movingPointStripPlugin_Fun);

		static uint8_t hue = 0;

		lcd.setCursor(0, 1);

		switch (eventReceived)
		{
		case Event::ChannelPlus:
			// zmiana aktywnej funkcji na nastepna
			if (fun + 1 == eoeFun)
				fun = firstFun;
			else
				fun++;
			storage.movingPointStripPlugin_Fun =
				static_cast<Storage::MovingPointStripPlugin_Fun>(fun);
			break;

		case Event::ChannelMinus:
			// zmiana aktywnej funkcji na poprzednia
			if (fun == firstFun)
				fun = eoeFun - 1;
			else
				fun--;
			storage.movingPointStripPlugin_Fun =
				static_cast<Storage::MovingPointStripPlugin_Fun>(fun);
			break;

		case Event::VolumePlus:
		case Event::VolumeMinus:
		case Event::Digit0:
		case Event::Digit1:
		case Event::Digit2:
		case Event::Digit3:
		case Event::Digit4:
		case Event::Digit5:
		case Event::Digit6:
		case Event::Digit7:
		case Event::Digit8:
		case Event::Digit9:
			// zwiekszenie wartosci aktywnej funkcji
			switch (storage.movingPointStripPlugin_Fun)
			{
			case Storage::MovingPointStripPlugin_Fun::SetHue:
				SetHue(eventReceived);
				break;
			case Storage::MovingPointStripPlugin_Fun::SetSpeed:
			case Storage::MovingPointStripPlugin_Fun::SetBrightness:
				break;
			}
			break;


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
			//case Event::Name::VolumePlus:
			//	hue += 5;
			//	lcd.printPropPerc(f_hue, hue * 100 / 255);
			//	movingColor.setHue(hue);
			//	break;
			//case Event::Name::VolumeMinus:
			//	hue -= 5;
			//	lcd.printPropPerc(f_hue, hue * 100 / 255);
			//	movingColor.setHue(hue);
			//	break;
		}

		return false;
	}

	void SetHue(Event::Name eventName)
	{
		const __FlashStringHelper* f_hue = F("hue");

		switch (eventName)
		{
		case Event::Digit0: storage.movingPointStripPlugin_Hue = PercentToByte(0); break;
		case Event::Digit1: storage.movingPointStripPlugin_Hue = PercentToByte(10); break;
		case Event::Digit2: storage.movingPointStripPlugin_Hue = PercentToByte(20); break;
		case Event::Digit3: storage.movingPointStripPlugin_Hue = PercentToByte(30); break;
		case Event::Digit4: storage.movingPointStripPlugin_Hue = PercentToByte(40); break;
		case Event::Digit5: storage.movingPointStripPlugin_Hue = PercentToByte(50); break;
		case Event::Digit6: storage.movingPointStripPlugin_Hue = PercentToByte(60); break;
		case Event::Digit7: storage.movingPointStripPlugin_Hue = PercentToByte(70); break;
		case Event::Digit8: storage.movingPointStripPlugin_Hue = PercentToByte(80); break;
		case Event::Digit9: storage.movingPointStripPlugin_Hue = PercentToByte(90); break;
		case Event::VolumePlus: storage.movingPointStripPlugin_Hue += PercentToByte(1); break;
		case Event::VolumeMinus: storage.movingPointStripPlugin_Hue -= PercentToByte(1); break;
		}
		lcd.printPropPerc(
			f_hue,
			static_cast<byte>(storage.movingPointStripPlugin_Hue * 100 / 255));
		movingColor.setHue(storage.movingPointStripPlugin_Hue);
	}


	byte PercentToByte(byte percent)
	{
		return static_cast<byte>(round(percent * 255 / 100.00));
	}

	// ustawia wszystkie ledy w kolory teczy
	void FillRainbow()
	{
		//float deltaHue = 255 / (float)strip.controller->size();
		byte deltaHue = 255 / (movingLastLedNo - movingFirstLedNo + 1);
		fill_rainbow(strip.controller->leds(), strip.controller->size(), 0, deltaHue);
	}

};
