#include <stdio.h>
#include "HardwareSerial.h"
//#include "EmdrWrapper.h"

size_t HardwareSerial::write(const char* str)
{
	//EmdrWrapper::Serial::OnWrite(str);
	return strlen(str);
}

size_t HardwareSerial::print(const char* str)
{
	//EmdrWrapper::Serial::OnWrite(str);
	return strlen(str);
}

size_t HardwareSerial::println(const char* str)
{
	//EmdrWrapper::Serial::OnWrite(str);
	//EmdrWrapper::Serial::OnWrite("\n");
	return strlen(str + 1);
}

size_t HardwareSerial::println()
{
	//EmdrWrapper::Serial::OnWrite("\n");
	return 1;
}


size_t HardwareSerial::print(int value, int format)
{
	char str[256] = "";
	snprintf(str, sizeof(str), "%d", value);
	//EmdrWrapper::Serial::OnWrite(str);
	return strlen(str);
}

size_t HardwareSerial::println(int value, int format)
{
	char str[256] = "";
	snprintf(str, sizeof(str), "%d\n", value);
	//EmdrWrapper::Serial::OnWrite(str);
	return strlen(str);
}


HardwareSerial Serial;