#pragma once
#include <Arduino.h>
#include <vector>
#include <map>
#include <IRremote.h>
#include "Event.h"

//extern IRrecv irrecv;

class RemoteController
{
public:
	enum Button
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

	RemoteController(IRrecv irrecv) : irrecv(irrecv)
	{
		irrecv.enableIRIn();
	}

	class EventArgs
	{
	public:

		EventArgs(Button button)
		{
			this->button = button;
		}

		Button button;
	};

	void ProcessCode(unsigned long code)
	{
		Button button = UNSUPPORTED;
		Serial.print(code, HEX);

		auto it = codeMapper.find(code);
		if (it != codeMapper.end())
			button = it->second;
		Serial.print("-> buttonPressed.Emit(");
		Serial.print(button);
		Serial.println(")");
		buttonPressed.Emit(EventArgs(button));
	}

	void Loop()
	{
		decode_results results;
		if (irrecv.decode(&results)) {
			ProcessCode(results.value);
			irrecv.resume(); // Continue receiving
		}
	}

	Event<EventArgs> buttonPressed;

private:
	IRrecv &irrecv;
	typedef const std::map<unsigned long, Button> CodeMapper;
	static CodeMapper AVerMediaCodeMapper;
	static CodeMapper _3939CodeMapper;
	static CodeMapper &codeMapper;
};
