#pragma once
#include <stdint.h>
#include "Emdr.h"
#include "EmdrController/EmdrController.h"
#include "EmdrController/Event.h"
#include "EmdrController/RemoteController.h"

using System::String;

// https://www.codeproject.com/Articles/19354/Quick-C-CLI-Learn-C-CLI-in-less-than-10-minutes#A8
namespace EmdrWrapper {

	public ref class EmdrSketch
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

		ref class Serial
		{
		public:
			delegate void writeDelegate(String^ text);
			static event writeDelegate^ writeEvent;
			static void OnWrite(const char* text)
			{
				writeEvent(gcnew String(text));
			}
		};

		ref class FastLED
		{
		public:
			delegate void addLedsDelegate(int ledsCount);
			static event addLedsDelegate^ addLedsEvent;
			static void OnAddLeds(int ledsCount)
			{
				addLedsEvent(ledsCount);
			}

			delegate void showDelegate(const uint8_t* leds, int ledsCount);
			static event showDelegate^ showEvent;
			static void OnShow(const uint8_t* leds, int nLeds)
			{
				pin_ptr<const uint8_t> pinned = leds;
				showEvent(pinned, nLeds);
			}
		};
	};

	public ref class EmdrControllerSketch
	{
	public:
		static void setup()
		{
			::setup_controller();
		}

		static void loop()
		{
			::loop_controller();
		}

		ref class irrecv
		{
		public:
			static void DecodeReturnValue(int value)
			{
				::remoteController.irrecv._decodeReturnValue = (unsigned int)value;
			}
		};

		ref class Event
		{
		public:
			delegate void sendDelegate(int name);
			static event sendDelegate^ sendEvent;
			static void OnSend(int name)
			{
				sendEvent(name);
			}
		};

		/*ref class RemoteController
		{
		public:
			static int EventNameToIRCode(int name)
			{
				return ::remoteController.EventNameToIRCode(name);
		
			}
		};*/

	};
}

