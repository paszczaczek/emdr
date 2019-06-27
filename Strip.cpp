#define FASTLED_INTERNAL
//#include <bitswap.h>
//#include <chipsets.h>
//#include <color.h>
//#include <colorpalettes.h>
//#include <colorutils.h>
//#include <controller.h>
//#include <cpp_compat.h>
//#include <dmx.h>
//#include <FastLED.h>
#include <FastLED.h>
//#include <fastled_config.h>
//#include <fastled_delay.h>
//#include <fastled_progmem.h>
//#include <fastpin.h>
//#include <fastspi.h>
//#include <fastspi_bitbang.h>
//#include <fastspi_dma.h>
//#include <fastspi_nop.h>
//#include <fastspi_ref.h>
//#include <fastspi_types.h>
//#include <hsv2rgb.h>
//#include <led_sysdefs.h>
//#include <lib8tion.h>
//#include <noise.h>
//#include <pixelset.h>
#include <pixeltypes.h>
//#include <platforms.h>
//#include <power_mgt.h>

#include "Strip.h"

Strip::Strip()
{
	leds = new CRGB[ledsCount];
	FastLED.addLeds<WS2811, 7, GRB>(leds, ledsCount);
	//FastLED.showColor(CRGB::Black);
}

Strip::~Strip()
{
	delete[] leds;
}

void Strip::Loop()
{
	if (ledCurrent < ledsCount)
	{
		leds[ledCurrent].r = 255;
		leds[ledCurrent].g = 0;
		leds[ledCurrent].b = 0;
		ledCurrent++;
	}
	FastLED.show();
}