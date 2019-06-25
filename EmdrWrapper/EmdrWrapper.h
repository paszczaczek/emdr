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
	};
}

