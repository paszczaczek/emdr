#pragma once
#include <stdint.h>
#include "Emdr.h"
#include "EmdrController/EmdrController.h"
#include "EmdrController/Event.h"
#include "EmdrController/RemoteController.h"
#include "Wire.h"

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

		ref class lcd
		{
		public:
			delegate void printDelegate(String^ text);
			static event printDelegate^ printEvent;
			static size_t OnPrint(const char text[])
			{
				printEvent(gcnew String(text));
				return strlen(text);
			}

			delegate void setCursorDelegate(uint8_t col, uint8_t row);
			static event setCursorDelegate^ setCursorEvent;
			static void OnSetCursor(uint8_t col, uint8_t row) 
			{
				setCursorEvent(col, row);
			}

			delegate void backlightDelegate(bool on);
			static event backlightDelegate^ backlightEvent;
			static void OnBacklight(bool on)
			{
				backlightEvent(on);
			}

			delegate void clearDelegate();
			static event clearDelegate^ clearEvent;
			static void OnClear()
			{
				clearEvent();
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
				::Wire._is_available = true;
				::Wire._available_data = name;
				::onReceiveFromController(0);
			}
		};
	};
}

