#pragma once
#include "../emdr.h"

using namespace System;

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

		delegate void serialWriteDelegate(String^ text);
		static event serialWriteDelegate^ serialWriteEvent;
		static void OnSerialWrite(const char *text) 
		{
			serialWriteEvent(gcnew String(text));
		}

		delegate void fastLEDShowDelegate(uint8_t *leds, int ledsCount);
		static event fastLEDShowDelegate^ fastLEDShowEvent;
		static void OnFastLEDShow()
		{
			/*
			(strip.leds + 0)->r = 255;
			(strip.leds + 0)->g = 0;
			(strip.leds + 0)->b = 0;

			(strip.leds + 1)->r = 0;
			(strip.leds + 1)->g = 255;
			(strip.leds + 1)->b = 0;

			(strip.leds + 2)->r = 0;
			(strip.leds + 2)->g = 0;
			(strip.leds + 2)->b = 255;

			(strip.leds + 3)->r = 255;
			(strip.leds + 3)->g = 255;
			(strip.leds + 3)->b = 255;

			(strip.leds + 4)->r = 0;
			(strip.leds + 4)->g = 0;
			(strip.leds + 4)->b = 0;
			*/

			pin_ptr<uint8_t> pinned = strip.leds->raw;
			fastLEDShowEvent(pinned, strip.ledsCount);
		}
	};
}

