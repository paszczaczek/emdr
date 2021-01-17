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
		//this->movingTimerCountTo = movingTimerCountTo;
		this->movingTimerCountTo = storage.movingPointStripPlugin_Speed;
		this->movingFirstLedNo = movingFirstLedNo;
		this->movingLastLedNo = movingLastLedNo;
		movingColor.setHue(storage.movingPointStripPlugin_Hue);
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

			lcd.printFun(F("moving point"));
			Receive(Event::Display);

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
	// obsluga eventow
	bool Receive(Event::Name receivedEvent) override
	{
		constexpr byte eoeFun = static_cast<byte>(Storage::MovingPointStripPlugin_Fun::EOE);
		constexpr byte firstFun = 0;
		byte fun = static_cast<byte>(storage.movingPointStripPlugin_Fun);
		const __FlashStringHelper* f_speed = F("speed");
		const __FlashStringHelper* f_hue = F("hue");
		const __FlashStringHelper* f_brightness = F("brightness");

		static byte value = 0;

		switch (receivedEvent)
		{
		case Event::VolumePlus:
			// zmiana aktywnej funkcji na nastepna
			if (fun + 1 == eoeFun)
				fun = firstFun;
			else
				fun++;
			storage.movingPointStripPlugin_Fun =
				static_cast<Storage::MovingPointStripPlugin_Fun>(fun);
			goto Display;

		case Event::VolumeMinus:
			// zmiana aktywnej funkcji na poprzednia
			if (fun == firstFun)
				fun = eoeFun - 1;
			else
				fun--;
			storage.movingPointStripPlugin_Fun =
				static_cast<Storage::MovingPointStripPlugin_Fun>(fun);
			goto Display;

		Display:
		case Event::Display:
		case Event::ChannelPlus:
		case Event::ChannelMinus:
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
			case Storage::MovingPointStripPlugin_Fun::SetSpeed:
				value = storage.movingPointStripPlugin_Speed;
				value = SetFunValueLog(value, receivedEvent, f_speed);
				if (receivedEvent != Event::Display)
					movingTimerCountTo = storage.movingPointStripPlugin_Speed = value;
				break;
			case Storage::MovingPointStripPlugin_Fun::SetHue:
				storage.movingPointStripPlugin_Hue =
					SetFunValue(storage.movingPointStripPlugin_Hue, receivedEvent, f_hue);
					movingColor.setHue(storage.movingPointStripPlugin_Hue);
					strip.controller->leds()[movingLedNo] = movingColor;
					strip.updated = true;
				break;
			case Storage::MovingPointStripPlugin_Fun::SetBrightness:
				storage.movingPointStripPlugin_Brightness =
					SetFunValueLog(storage.movingPointStripPlugin_Brightness, receivedEvent, f_brightness);
					strip.SetBrightness(storage.movingPointStripPlugin_Brightness);
				break;
			}
			break;
		case Event::Name::UnknowCode:
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

		strip.updated = true;

		return false;
	}

	byte SetFunValue(short value, Event::Name eventName, const __FlashStringHelper* lcdName)
	{
		switch (eventName)
		{
		case Event::Display: break;
		case Event::Digit0: value = 0; break; 
		case Event::Digit1: value = 25 * 1; break;
		case Event::Digit2: value = 25 * 2; break;
		case Event::Digit3: value = 25 * 3; break;
		case Event::Digit4: value = 25 * 4; break;
		case Event::Digit5: value = 25 * 5; break;
		case Event::Digit6: value = 25 * 6; break;
		case Event::Digit7: value = 25 * 7; break;
		case Event::Digit8: value = 25 * 8; break;
		case Event::Digit9: value = 25 * 9; break;
		case Event::ChannelPlus: value += 1; break;
		case Event::ChannelMinus: value -= 1; break;
		}
		value = max(0, value);
		value = min(255, value);

		lcd.printProp(lcdName, value);

		return static_cast<byte>(value);
	}

	byte SetFunValueLog(short value, Event::Name eventName, const __FlashStringHelper* lcdName)
	{
		switch (eventName)
		{
		case Event::Display: break;
		case Event::Digit0: value = 0; break;
		case Event::Digit1: value = 1; break;
		case Event::Digit2: value = 2; break;
		case Event::Digit3: value = 4; break;
		case Event::Digit4: value = 8; break;
		case Event::Digit5: value = 16; break;
		case Event::Digit6: value = 32; break;
		case Event::Digit7: value = 64; break;
		case Event::Digit8: value = 128; break;
		case Event::Digit9: value = 255; break;
		case Event::ChannelPlus: value += 1; break;
		case Event::ChannelMinus: value -= 1; break;
		}
		value = max(0, value);
		value = min(255, value);

		lcd.printProp(lcdName, value);

		return static_cast<byte>(value);
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
