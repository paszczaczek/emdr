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

#include <pixeltypes.h>
#include "Strip.h"
#include "EventHandler.h"
#include "EventArgs.h"

Strip::Strip()
{
	leds = new CRGB[ledsCount];
	ledsController = &FastLED.addLeds<WS2811, 7, GRB>(leds, ledsCount);
	//ledsController->showColor(CRGB::Black);

	movingTimer.elapsed += new EventHandler<Strip, TimerEventArgs>(this, &Strip::onMovingEvent);
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

  Serial.write("onMovindEvent");

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
	(leds + ledCurrent)->g = 0;
	(leds + ledCurrent)->b = 0;

	(leds + ledNext)->r = 0;
	(leds + ledNext)->g = 0;
	(leds + ledNext)->b = 0;

	FastLED.show();
	ledCurrent = ledNext;
}
