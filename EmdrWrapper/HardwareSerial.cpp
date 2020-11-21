#include <stdio.h>
#include "HardwareSerial.h"
#include "EmdrWrapper.h"

size_t HardwareSerial::write(const char *str)
{
	EmdrWrapper::EmdrSketch::Serial::OnWrite(str);
	return strlen(str);
}

size_t HardwareSerial::print(const char *str)
{
	EmdrWrapper::EmdrSketch::Serial::OnWrite(str);
	return strlen(str);
}

size_t HardwareSerial::println(const char *str)
{
	EmdrWrapper::EmdrSketch::Serial::OnWrite(str);
	EmdrWrapper::EmdrSketch::Serial::OnWrite("\n");
	return strlen(str + 1);
}

size_t HardwareSerial::println()
{
	EmdrWrapper::EmdrSketch::Serial::OnWrite("\n");
	return 1;
}


size_t HardwareSerial::print(int value, int format)
{
	char str[256] = "";
	if (format == HEX)
		snprintf(str, sizeof(str), "0x%x", value);
	else
		snprintf(str, sizeof(str), "%d", value);
	EmdrWrapper::EmdrSketch::Serial::OnWrite(str);
	return strlen(str);
}

size_t HardwareSerial::println(int value, int format)
{
	char str[256] = "";
	snprintf(str, sizeof(str), "%d\n", value);
	EmdrWrapper::EmdrSketch::Serial::OnWrite(str);
	return strlen(str);
}


HardwareSerial Serial;