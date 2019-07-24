#pragma once
#include "StripPlugin.h"
#include "Timer.h"

class MovingPointStripPlugin : public StripPlugin
{
public:
	MovingPointStripPlugin()
	{
		movingTimer.elapsed += new EventHandler<MovingPointStripPlugin, Timer::EventArgs>
			(this, &MovingPointStripPlugin::onMovingEvent);
		movingTimer.interval = 1000;
		movingTimer.Start();
	}

	virtual void Loop() override
	{
		movingTimer.Loop();
	}

private:
	Timer movingTimer;
	int ledCurrent = 0;
	enum MovingDirection { RIGTH, LEFT } movingDirection = RIGTH;;

	void onMovingEvent(Timer::EventArgs& args)
	{
		auto ledNext = ledCurrent;

		if (movingDirection == MovingDirection::RIGTH)
			if (ledCurrent < strip->controller->size() - (int)args.elapsedIntervals)
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

		strip->controller->leds()[ledCurrent] = CRGB::Black;
		strip->controller->leds()[ledNext] = CRGB::Orange;
		strip->updated = true;
		//Serial.println("MovingPointStripPlugin::onMovingEvent");

		ledCurrent = ledNext;
	}
};
