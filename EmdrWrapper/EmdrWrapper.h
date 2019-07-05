#pragma once
#include <stdint.h>
#include "Emdr.h"

#include "Event.h"
#include "RemoteController.h"

using System::String;

// https://www.codeproject.com/Articles/19354/Quick-C-CLI-Learn-C-CLI-in-less-than-10-minutes#A8
namespace EmdrWrapper {

	public ref class Sketch
	{
	public:
		static void setup() 
		{
			::setup(); 
		}

		static void loop() 
		{ 
			::loop(); 
		}

		static void RemoteControllerCodeReceived(int code) 
		{
			remoteController.ProcessCode(code);
		}

		delegate void serialWriteDelegate(String^ text);
		static event serialWriteDelegate^ serialWriteEvent;
		static void OnSerialWrite(const char *text) 
		{
			serialWriteEvent(gcnew String(text));
		}

		delegate void fastLEDShowDelegate(const uint8_t *leds, int ledsCount);
		static event fastLEDShowDelegate^ fastLEDShowEvent;
		static void OnFastLEDShow(const uint8_t *leds, int nLeds)
		{
			pin_ptr<const uint8_t> pinned = leds;
			fastLEDShowEvent(pinned, nLeds);
		}
	};
}

