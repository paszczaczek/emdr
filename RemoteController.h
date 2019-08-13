#pragma once
#include <stdint.h>
#include <IRremote.h>
#include "Debug.h"
#include "Event.h"
//#include "Strip.h"

class RemoteController
{
public:
	RemoteController(IRrecv *irrecv, decode_results *results) :
		event(2), // jesli dodasz nowego subskrybenta, zwieksz wartosc
		irrecv(irrecv),
		results(results),
		badCodeCount(0)
	{
	}

	enum struct Button : byte
	{
		UNSUPPORTED = 0,
		PAUSE_REQUEST,
		RESUME_REQUEST,
		CHANEL_PLUS,
		CHANEL_MINUS,
		PAUSE,
		PLAY,
		DIGIT_1,
		DIGIT_2,
		DIGIT_3
	};

	class EventArgs
	{
	public:
		Button button;
	};

	void ProcessCode(unsigned long code) {
		Button button = Button::UNSUPPORTED;
		bool codeFound = false;
		for (int i = 0; i < RemoteController::codeMapperCount; i++)
			if (codeMapper[i].code == code)
			{
				// znaleziono pasujacy kod
				if (badCodeCount > 0)
				{
					badCodeCount = 0;
					PRINTLN(F("IR: RESUME_REQUEST!"));
					args.button = Button::RESUME_REQUEST;
					event.Emit(args);
				}
				button = codeMapper[i].button;
				codeFound = true;
				break;
			}

		if (!codeFound)
		{
			if (++badCodeCount >= 10)
			{
				// Jesli ciagle kody sa niezrozumiale, to przyczyna moze byc czeste
				// odswiezanie stripu, bo FastLED blokuje przerwania na czas programowania diod.
				// Wysylamy wiec komunikat blokujacy pluginy, tak aby kod mogl zostac poprawnie
				// odczytany.
				PRINTLN(F("IR: PAUSE_REQUEST!"));
				button = Button::PAUSE_REQUEST;
				//badCodeCount = 0;
			}
		}

		PRINT(F("IR: ")); PRINT(code, HEX); PRINT(F(" -> ")); PRINT((int)button); PRINT(" ("); PRINT(badCodeCount); PRINTLN(")");
		args.button = button;
		event.Emit(args);
	}

	void Loop() {
		if (irrecv->decode(results)) {
			ProcessCode(results->value);
			irrecv->resume();
		}
	}

	Event<EventArgs> event;

private:
	IRrecv *irrecv;
	decode_results *results;
	EventArgs args;
	byte badCodeCount;
	typedef const struct {
		unsigned long code;
		Button button;
	} CodeMapper[];
	static CodeMapper codeMapper;
	static byte codeMapperCount;
};

extern RemoteController remoteController;
