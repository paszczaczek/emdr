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
#include "EventHandler.h"
#include "EventArgs.h"

Strip::Strip()
	: movingEventHandler(this, &Strip::onMovingEvent)
{
	leds = new CRGB[ledsCount];
	FastLED.addLeds<WS2811, 7, GRB>(leds, ledsCount);
	//FastLED.showColor(CRGB::Black);

	movingTimer.elapsed += movingEventHandler;
	movingTimer.interval = 20;
	movingTimer.Start();
}

Strip::~Strip()
{
	delete[] leds;
}

void Strip::Loop()
{
	movingTimer.Loop();
}

void Strip::onMovingEvent(TimerEventArgs& args)
{
	auto ledNext = ledCurrent;

	if (movingDirection == MovingDirection::RIGTH)
		if (ledCurrent < ledsCount - args.elapsedIntervals)
			ledNext += args.elapsedIntervals;
		else
			movingDirection = MovingDirection::LEFT;
	else
		if (ledCurrent > args.elapsedIntervals - 1)
			ledNext -= args.elapsedIntervals;
		else
			movingDirection = MovingDirection::RIGTH;

	if (ledNext == ledCurrent)
		return;

	(leds + ledCurrent)->r = 255;
	(leds + ledCurrent)->g = 255;
	(leds + ledCurrent)->b = 255;

	(leds + ledNext)->r = 255;
	(leds + ledNext)->g = 0;
	(leds + ledNext)->b = 0;

	FastLED.show();
	ledCurrent = ledNext;
}