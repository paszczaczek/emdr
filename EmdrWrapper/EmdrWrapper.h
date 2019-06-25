#pragma once
#include "../emdr.h"
using namespace System;

// https://www.codeproject.com/Articles/19354/Quick-C-CLI-Learn-C-CLI-in-less-than-10-minutes#A8
namespace EmdrWrapper {
	public ref class Sketch
	{
		// TODO: Add your methods for this class here.
	public:
		static void setup() 
		{
			::setup();
		}
	};
}
