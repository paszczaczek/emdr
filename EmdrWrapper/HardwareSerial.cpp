#include "HardwareSerial.h"
#include "EmdrWrapper.h"

size_t HardwareSerial::write(const char *str)
{
	EmdrWrapper::Serial::OnWrite(str);
	return strlen(str);
}

size_t HardwareSerial::print(const char *str)
{
	EmdrWrapper::Serial::OnWrite(str);
	return strlen(str);
}

size_t HardwareSerial::println(const char *str)
{
	// TODO: \n
	EmdrWrapper::Serial::OnWrite(str);
	return strlen(str);
}

size_t HardwareSerial::println(int value, int format)
{
	char str[256] = "";
	snprintf(str, sizeof(str), "%d", value);
	EmdrWrapper::Serial::OnWrite(str);
	return strlen(str);
}


HardwareSerial Serial;