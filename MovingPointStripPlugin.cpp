#define FASTLED_INTERNAL
#include <FastLED.h>
#include "MovingPointStripPlugin.h"
#include "Strip.h"

MovingPointStripPlugin::MovingPointStripPlugin()
{
	movingTimer.elapsed += new EventHandler<MovingPointStripPlugin, TimerEventArgs>
		(this, &MovingPointStripPlugin::onMovingEvent);
	movingTimer.interval = 100;
	movingTimer.Start();
}

void MovingPointStripPlugin::Loop() 
{
	movingTimer.Loop();
}

void MovingPointStripPlugin::onMovingEvent(TimerEventArgs& args)
{
	auto ledNext = ledCurrent;

	if (movingDirection == MovingDirection::RIGTH)
		if (ledCurrent < strip->ledsCount - (int)args.elapsedIntervals)
			ledNext += args.elapsedIntervals;
		else
			movingDirection = MovingDirection::LEFT;
	else
		if (ledCurrent > (int)args.elapsedIntervals - 1)
			ledNext -= args.elapsedIntervals;
		else
			movingDirection = MovingDirection::RIGTH;

	if (ledNext == ledCurrent)
		return;

	strip->ledsController->leds()[ledCurrent] = CRGB::Black;
	strip->ledsController->leds()[ledNext] = CRGB::Orange;

	strip->ledsController->showLeds();

	ledCurrent = ledNext;
}

