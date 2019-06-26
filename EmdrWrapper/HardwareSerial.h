#pragma once
#include <string.h>

class HardwareSerial //: public Stream
{
public:
	size_t write(const char *str);
};
extern HardwareSerial Serial;