#pragma once
#include <stdint.h>
#include <IRremote.h>
#include "Debug.h"
#include "Timer.h"
#include "Event.h"
//#include "Strip.h"

class RemoteController
{
public:
	RemoteController(IRrecv *irrecv, decode_results *results) :
		//event(2), // jesli dodasz nowego subskrybenta, zwieksz wartosc
		irrecv(irrecv),
		results(results)
	{
	}

	void ProcessCode(unsigned long code) {
		PRINT(F("IR: ")); PRINT(code, HEX); PRINT(F(" -> "));
		// probujemy rozpoznac kod
		for (int i = 0; i < RemoteController::codeMapperCount; i++)
		{
			if (codeMapper[i].code == code)
			{
				// kod rozpoznany
				args.button = codeMapper[i].button;
				PRINTLN((int)args.button);
				//event.Emit(args);
				Event::Send(Event::Type::SELECT_STRIP_DEVICE);
				return;
			}
		}

		// kod nierozpoznany
		if (!intBlkDisallowed)
		{
			// kod nierozpoznay ale nie wiadomo czy dlatego ze:
			// a. strip zablokowal przerwania przeklamujac odczyt kodu
			// b. odczyt kodu jest poprawny ale go nie obslugujemy
			// wysylamy zakaz uzywania przerwan, aby by to rozstrzygnac
			args.button = Button::IntBlkDisallowed;
			PRINT((int)args.button); PRINTLN(F("(IntBlkDisallowed)"));
			//releaseInterruptsTimer.Start();
			intBlkDisallowed = true;
		}
		else
		{
			// kod nierozpoznany i wyslalismy juz wczesniej zakaz uzywania przerwan 
			// oznacza to ze kod odczytal sie poprawnie, ale nie obslugujemy 
			// wysylamy komunikat o nieznanym kodzie
			args.button = Button::Unknown;
			PRINT((int)args.button); PRINTLN(F("(Unknown)"));
		}

		//event.Emit(args);
	}

	void Loop() {
		// jesli jest zakaz uzywania przerwan i dosc dlugo nie odebrano zadnego kodu,
		if (intBlkDisallowed && millis() - lastCodeTime > 3000)
		{
			// to prawdopodobnie pilot nie bedzie w najblizszej chwili uzywany 
			// uchylam zakaz uzywania przerwan
			args.button = Button::IntBlkAllowed;
			PRINT(F("IR: timeout -> ")); PRINT((int)args.button); PRINTLN(F("(IntBlkAllowed)"));
			//event.Emit(args);
			intBlkDisallowed = false;
		}
		if (irrecv->decode(results)) {
			ProcessCode(results->value);
			lastCodeTime = millis();
			irrecv->resume();
		}
	}

	enum struct Button : byte
	{
		Unknown = 0,
		IntBlkDisallowed,
		IntBlkAllowed,
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

	//Event<EventArgs> event;

private:
	IRrecv *irrecv;
	decode_results *results;
	EventArgs args;
	unsigned long lastCodeTime = 0;
	bool intBlkDisallowed = false;

	typedef const struct {
		unsigned long code;
		Button button;
	} CodeMapper[];
	static CodeMapper codeMapper;
	static byte codeMapperCount;


};

extern RemoteController remoteController;
