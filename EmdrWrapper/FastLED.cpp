#include "FastLED.h"
#include "EmdrWrapper.h"
#include "WrapperController.h"

std::vector<WrapperController*> CFastLED::controllers;

void CFastLED::show()
{
	EmdrWrapper::Sketch::OnFastLEDShow();
}

CFastLED FastLED;
