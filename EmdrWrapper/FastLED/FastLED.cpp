#include "FastLED.h"
#include "EmdrWrapper.h"
#include "WrapperController.h"

std::vector<WrapperController*> CFastLED::controllers;

void CFastLED::show()
{
	EmdrWrapper::EmdrSketch::FastLED::OnShow(leds()->raw, size());
}

void CFastLED::show(uint8_t scale)
{
	EmdrWrapper::EmdrSketch::FastLED::OnShow(leds()->raw, size());
}

void CFastLED::clear(bool writeData) {}

void CFastLED::setBrightness(uint8_t scale)
{
	//TODO
	//EmdrWrapper::EmdrSketch::FastLED::OnSetBrightness(scale);
	m_Scale = scale;
}

uint8_t CFastLED::getBrightness()
{
	//TODO
	//EmdrWrapper::EmdrSketch::FastLED::OnGetBrightness(scale);
	return m_Scale;
}

CFastLED FastLED;
