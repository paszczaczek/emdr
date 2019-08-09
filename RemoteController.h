#pragma once
#include <stdint.h>
//#include <Arduino.h>
#include "Debug.h"
#include <IRremote.h>
#include "Event.h"

class RemoteController
{
public:
	RemoteController(IRrecv *irrecv, decode_results *results) :
		irrecv(irrecv),
		results(results) {}

	enum Button : uint8_t
	{
		UNSUPPORTED = 0,
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
		//  EventArgs(Button button) {
		//    this->button = button;
		//  }
		Button button;
	};

	void ProcessCode(unsigned long code) {
		Button button = UNSUPPORTED;
		for (int i = 0; ; i++) {
			if (codeMapper[i].code == 0)
				break;
			if (codeMapper[i].code == code)
				button = codeMapper[i].button;
		}
		PRINT(("IR: ")); PRINT(code, HEX); PRINT((" -> ")); PRINT(button); PRINTLN("");
		args.button = button;
		//buttonPressed.Emit(EventArgs(button));
		buttonPressed.Emit(args);
	}

	void Loop() {
		if (irrecv->decode(results)) {
			ProcessCode(results->value);
			irrecv->resume();
		}
	}

	Event<EventArgs> buttonPressed;

private:
	IRrecv *irrecv;
	decode_results *results;
	EventArgs args;
	typedef const struct {
		unsigned long code;
		Button button;
	} CodeMapper[];
	static CodeMapper codeMapper;
};

extern RemoteController remoteController;