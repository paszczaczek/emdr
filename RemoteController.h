#pragma once
//#include <stdint.h>
#include <IRremote.h>
#include "Debug.h"
#include "Timer.h"
#include "Event.h"

extern IRrecv irrecv;

class RemoteController
{
private:
	// odczytany kod z pilota
	decode_results results;

	// czas odczytania ostatniego kodu z pilota
	unsigned long lastCodeTime = 0;

	// czy zakaz blokowania przerwan obiwiazuje
	bool isBlockingInterruptsDisallowed = false;

public:
	void Loop() {
		// jesli jest zakaz blokowania przerwan i dosc dlugo nie odebrano zadnego kodu,
		if (isBlockingInterruptsDisallowed && millis() - lastCodeTime > 3000)
		{
			// to prawdopodobnie pilot nie bedzie w najblizszej chwili uzywany 
			// uchylam zakaz uzywania przerwan
			PRINT(F("IR: timeout -> ")); PRINTLN((int)Event::Name::BlockingInterruptsAllowed);
			isBlockingInterruptsDisallowed = false;
			Event::Send(Event::Name::BlockingInterruptsAllowed);
		}
		if (irrecv.decode(&results)) {
			ProcessCode();
			lastCodeTime = millis();
			irrecv.resume();
		}
	}

private:

	// przetwarza kod odczytany z pilota
	void ProcessCode() {
		PRINT(F("IR: ")); PRINT(results.value, HEX); PRINT(F(" -> "));

		// probujemy rozpoznac kod
		Event::Name eventName = GetEventName();
		if (eventName != Event::Name::UnknowCode)
		{
			// kod rozpoznany
			PRINTLN((byte)eventName);
			Event::Send(eventName);
			return;
		}

		// kod nierozpoznany
		if (!isBlockingInterruptsDisallowed)
		{
			// kod nierozpoznay i nie wiadomo czy dlatego ze:
			// a. strip zablokowal przerwania przeklamujac odczyt kodu
			// b. odczyt kodu jest poprawny ale go nie obslugujemy
			// wysylamy zakaz blokowania przerwan, aby by to rozstrzygnac
			isBlockingInterruptsDisallowed = true;
			PRINT(F("UNKNOWN -> BLOCK"));
			Event::Send(Event::Name::UnknowCode);
			Event::Send(Event::Name::BlockingInterruptsDisallowed);
		}
		else
		{
			// kod nierozpoznany i wyslalismy juz wczesniej zakaz uzywania przerwan 
			// oznacza to ze kod odczytal sie poprawnie, ale nie obslugujemy 
			// wysylamy komunikat o nieznanym kodzie
			PRINTLN(F("UNKNOWN"));
			Event::Send(Event::Name::UnknowCode);
		}
	}

	// rozpoznaje kody z pilota
	Event::Name GetEventName()
	{
		switch (results.value)
		{
		case 0x175: case 0x975: return Event::Name::Start;
		case 0x176: case 0x976: return Event::Name::Stop;
		case 0x169: case 0x969: return Event::Name::Pause;

		case 0x160: case 0x960: return Event::Name::CHANEL_PLUS;
		case 0x161: case 0x961: return Event::Name::CHANEL_MINUS;

		default: return Event::Name::UnknowCode;
		}
	}
};

extern RemoteController remoteController;
