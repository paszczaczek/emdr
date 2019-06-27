#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <pixeltypes.h>
#include "Timer.h"
#include "EventArgs.h"

class Strip
{
public:
	int ledsCount = 180;
	CRGB *leds = nullptr;

	Strip();
	~Strip();
	void Loop();

private:
	uint8_t ledCurrent = 0;
	enum MovingDirection { RIGTH, LEFT } movingDirection = RIGTH;;

	EventHandler<TimerEventArgs, Strip> movingEventHandler;
	Timer movingTimer;
	void onMovingEvent(TimerEventArgs& args);
};
