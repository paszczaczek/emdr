#pragma once
#include <Arduino.h>
#include <IRremote.h>
#include "Event.h"
#include "IRCode.h"

class RemoteController
{
private:
	constexpr static int RC_PIN = 12;

public:
	RemoteController() : irrecv(RC_PIN) { }

	void Setup()
	{
		irrecv.enableIRIn();
		irrecv.blink13(true);
		Serial.println(F("ir enabled"));
	}

	void Loop() {
		if (irrecv.decode(&results)) {
			ProcessCode();
			irrecv.resume();
		}
	}

private:

	// przetwarza kod odczytany z pilota
	void ProcessCode() {
		Event::Name eventName = RecognizeIRCode();
		SerialPrintCodes(results.value, eventName);
		Event::Send(eventName);
		return;
	}

	// rozpoznaje kody z pilota
	Event::Name RecognizeIRCode()
	{
		switch (results.value)
		{
		case AverMediaIRCode::Digit1: return Event::Name::Digit1;
		case AverMediaIRCode::Digit2: return Event::Name::Digit2;
		case AverMediaIRCode::Digit3: return Event::Name::Digit3;
		case AverMediaIRCode::Digit4: return Event::Name::Digit4;
		case AverMediaIRCode::Digit5: return Event::Name::Digit5;
		case AverMediaIRCode::Digit6: return Event::Name::Digit6;
		case AverMediaIRCode::Digit7: return Event::Name::Digit7;
		case AverMediaIRCode::Digit8: return Event::Name::Digit8;
		case AverMediaIRCode::Digit9: return Event::Name::Digit9;
		case AverMediaIRCode::Digit0: return Event::Name::Digit0;
		default:                      return Event::Name::UnknowCode;
		}
	}

	void SerialPrintCodes(unsigned long irCode, Event::Name eventName)
	{
		Serial.print(F("ir: "));
		Serial.print(irCode, HEX);
		Serial.print(F(" -> "));
		switch (eventName)
		{
		case Event::UnknowCode: Serial.println(F("UnknowCode")); break;
		case Event::Digit1:     Serial.println(F("Digit1")); break;
		case Event::Digit2:     Serial.println(F("Digit2")); break;
		case Event::Digit3:     Serial.println(F("Digit3")); break;
		case Event::Digit4:     Serial.println(F("Digit4")); break;
		case Event::Digit5:     Serial.println(F("Digit5")); break;
		case Event::Digit6:     Serial.println(F("Digit6")); break;
		case Event::Digit7:     Serial.println(F("Digit7")); break;
		case Event::Digit8:     Serial.println(F("Digit8")); break;
		case Event::Digit9:     Serial.println(F("Digit9")); break;
		case Event::Digit0:     Serial.println(F("Digit0")); break;
		}
	}

public:
	//int EventNameToIRCode(int _name) {
	//	Event::Name name = static_cast<Event::Name>(_name);
	//	switch (name)
	//	{
	//	case Event::Start:      return IR_CODE_START;
	//	case Event::Stop:       return IR_CODE_STOP;
	//	case Event::Pause:      return IR_CODE_PAUSE;
	//	case Event::UnknowCode: return -1;
	//	default:                return -1;
	//	}
	//}
	
	// odbiornik pilota
	IRrecv irrecv;

private:
	// odczytany kod z pilota
	decode_results results;
};

extern RemoteController remoteController;
