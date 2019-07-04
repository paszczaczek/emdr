#pragma once
#include "FastLED.h"
#include "controller.h"

class WrapperController : public CLEDController {
	// Inherited via CLEDController
	virtual void showColor(const CRGB & data, int nLeds, CRGB scale) override {};
	virtual void show(const CRGB * data, int nLeds, CRGB scale) override {};
};