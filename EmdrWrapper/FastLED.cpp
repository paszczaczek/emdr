#include "FastLED.h"
#include "EmdrWrapper.h"
#include "WrapperController.h"

std::vector<WrapperController*> CFastLED::controllers;

void CFastLED::show()
{
	EmdrWrapper::FastLED::OnShow(leds()->raw, size());
}

void CFastLED::show(uint8_t scale)
{
	EmdrWrapper::FastLED::OnShow(leds()->raw, size());
}

void CFastLED::clear(bool writeData) {}

CFastLED FastLED;
