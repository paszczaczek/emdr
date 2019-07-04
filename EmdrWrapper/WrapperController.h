#pragma once
#include "FastLED.h"
#include "EmdrWrapper.h"

class WrapperController : public CLEDController {
	// Inherited via CLEDController
	virtual void showColor(const CRGB & data, int nLeds, CRGB scale) override 
	{
		CRGB *leds = this->leds();
		for (int i = 0; i < nLeds; i++)
			memcpy(leds + i, &data, sizeof(data));

		EmdrWrapper::Sketch::OnFastLEDShow(leds->raw, size());
	};

	virtual void show(const CRGB * data, int nLeds, CRGB scale) override 
	{
		//EmdrWrapper::Sketch::OnFastLEDShow(nullptr, 0);
		EmdrWrapper::Sketch::OnFastLEDShow(data->raw, nLeds);
	};
};