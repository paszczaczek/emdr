#include <Wire.h>
#include "Event.h"

#ifdef _WIN32
#include "EmdrWrapper.h"
#endif

constexpr int emdrI2CAddr = 9;

void Event::Send(Event::Name name)
{
#ifdef _WIN32
	EmdrWrapper::EmdrControllerSketch::Event::OnSend((int)name);
#else
	Wire.beginTransmission(emdrI2CAddr);
	Wire.write(name);
	Wire.endTransmission(emdrI2CAddr);
#endif
}