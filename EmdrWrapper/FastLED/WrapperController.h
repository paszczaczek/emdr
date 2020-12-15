#pragma once
#include "FastLED.h"
#include "EmdrWrapper.h"

class WrapperController : public CLEDController {
	virtual void showColor(const CRGB & data, int nLeds, CRGB scale) override 
	{
		CRGB *leds = this->leds();
		for (int i = 0; i < nLeds; i++)
			memcpy(leds + i, &data, sizeof(data));

		EmdrWrapper::EmdrSketch::FastLED::OnShow(leds->raw, size());
	};

	virtual void show(const CRGB * data, int nLeds, CRGB scale) override 
	{
		EmdrWrapper::EmdrSketch::FastLED::OnShow(data->raw, nLeds);
	};
};