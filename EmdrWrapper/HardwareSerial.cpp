#include "HardwareSerial.h"
#include "EmdrWrapper.h"

size_t HardwareSerial::write(const char *str)
{
	EmdrWrapper::Sketch::OnSerialWrite(str);

	return strlen(str);
}

HardwareSerial Serial;