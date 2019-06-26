#include "FastLED.h"
#include "EmdrWrapper.h"

void CFastLED::show()
{
	EmdrWrapper::Sketch::OnFastLEDShow();
}

CFastLED FastLED;