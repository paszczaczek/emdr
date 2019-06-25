#pragma once
#include <string.h>
#include "EmdrWrapper.h"

class HardwareSerial //: public Stream
{
public:
	size_t write(const char *str) 
	{
		EmdrWrapper::Sketch::OnSerialWrite(str);

		return strlen(str);
	}
};

HardwareSerial Serial;