#define FASTLED_INTERNAL
#include <FastLED.h>
#include "Strip.h"
#include "EventHandler.h"
#include "EventArgs.h"

Strip::Strip()
{
	movingTimer.elapsed += new EventHandler<Strip, TimerEventArgs>(this, &Strip::onMovingEvent);
	movingTimer.interval = 100;
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

	leds[ledCurrent] = CRGB::Black;
	leds[ledNext] = CRGB::Red;

	ledsController->showLeds();

	ledCurrent = ledNext;
}