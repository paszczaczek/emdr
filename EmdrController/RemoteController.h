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
		//Serial.println(F("rc: ir enabled"));
	}

	void Loop() {
		if (irrecv.decode(&results)) {
			ProcessCode();
			irrecv.resume();
		}
	}

	static void SerialPrintCodes(unsigned long irCode, Event::Name eventReceived)
	{
		if (irCode != -1) 
		{
			Serial.print(F("rc: ir: "));
			Serial.print(irCode, HEX);
			Serial.print(F(" -> "));
		}

		switch (eventReceived)
		{
		case Event::UnknowCode:		Serial.println(F("UnknowCode")); break;
		case Event::Digit1:			Serial.println(F("Digit1")); break;
		case Event::Digit2:			Serial.println(F("Digit2")); break;
		case Event::Digit3:			Serial.println(F("Digit3")); break;
		case Event::Digit4:			Serial.println(F("Digit4")); break;
		case Event::Digit5:			Serial.println(F("Digit5")); break;
		case Event::Digit6:			Serial.println(F("Digit6")); break;
		case Event::Digit7:			Serial.println(F("Digit7")); break;
		case Event::Digit8:			Serial.println(F("Digit8")); break;
		case Event::Digit9:			Serial.println(F("Digit9")); break;
		case Event::Digit0:			Serial.println(F("Digit0")); break;
		case Event::VolumePlus:		Serial.println(F("VolumePlus")); break;
		case Event::VolumeMinus:	Serial.println(F("VolumeMinus")); break;
		case Event::ChannelPlus:	Serial.println(F("ChannelPlus")); break;
		case Event::ChannelMinus:	Serial.println(F("ChannelMinus")); break;
		case Event::Power:			Serial.println(F("Power")); break;
		case Event::Mute:			Serial.println(F("Mute")); break;
		case Event::Display:		Serial.println(F("Display")); break;
		}
	}


private:

	// przetwarza kod odczytany z pilota
	void ProcessCode() {
		Event::Name eventReceived = RecognizeIRCode(results.value);
		if (eventReceived == Event::Name::UnknowCode && results.value >= 0x800)
			eventReceived = RecognizeIRCode(results.value - 0x800);
		SerialPrintCodes(results.value, eventReceived);
		Event::Send(eventReceived);

		return;
	}

	// rozpoznaje kody z pilota
	Event::Name RecognizeIRCode(unsigned long resultValue)
	{
		switch (resultValue)
		{
		case RC8205LX_IRCode::Digit1:		return Event::Name::Digit1;
		case RC8205LX_IRCode::Digit2:		return Event::Name::Digit2;
		case RC8205LX_IRCode::Digit3:		return Event::Name::Digit3;
		case RC8205LX_IRCode::Digit4:		return Event::Name::Digit4;
		case RC8205LX_IRCode::Digit5:		return Event::Name::Digit5;
		case RC8205LX_IRCode::Digit6:		return Event::Name::Digit6;
		case RC8205LX_IRCode::Digit7:		return Event::Name::Digit7;
		case RC8205LX_IRCode::Digit8:		return Event::Name::Digit8;
		case RC8205LX_IRCode::Digit9:		return Event::Name::Digit9;
		case RC8205LX_IRCode::Digit0:		return Event::Name::Digit0;
		case RC8205LX_IRCode::VolumePlus:   return Event::Name::VolumePlus;
		case RC8205LX_IRCode::VolumeMinus:  return Event::Name::VolumeMinus;
		case RC8205LX_IRCode::ChannelPlus:  return Event::Name::ChannelPlus;
		case RC8205LX_IRCode::ChannelMinus: return Event::Name::ChannelMinus;
		case RC8205LX_IRCode::Power:		return Event::Name::Power;
		case RC8205LX_IRCode::Mute:			return Event::Name::Mute;
		case RC8205LX_IRCode::Display:		return Event::Name::Display;
		default:							return Event::Name::UnknowCode;
		}
	}

public:
	// odbiornik pilota
	IRrecv irrecv;

private:
	// odczytany kod z pilota
	decode_results results;
};

extern RemoteController remoteController;
