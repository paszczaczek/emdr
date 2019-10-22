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
	};

	public ref class Serial 
	{
	public:
		delegate void writeDelegate(String^ text);
		static event writeDelegate^ writeEvent;
		static void OnWrite(const char *text)
		{
			writeEvent(gcnew String(text));
		}
	};

	public ref class RemoteController
	{
	public:
		static void ButtonReceived(int eventName)
		{
			Event::Send((Event::Name)eventName);
			//::RemoteController::EventArgs args;
			//args.button = (::RemoteController::Button)button;
			//remoteController.event.Emit(args);
		}
	};

	public ref class FastLED
	{
	public:
		delegate void addLedsDelegate(int ledsCount);
		static event addLedsDelegate^ addLedsEvent;
		static void OnAddLeds(int ledsCount)
		{
			addLedsEvent(ledsCount);
		}

		delegate void showDelegate(const uint8_t *leds, int ledsCount);
		static event showDelegate^ showEvent;
		static void OnShow(const uint8_t *leds, int nLeds)
		{
			pin_ptr<const uint8_t> pinned = leds;
			showEvent(pinned, nLeds);
		}
	};
}

