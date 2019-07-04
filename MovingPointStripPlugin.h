#pragma once
#include "StripPlugin.h"
#include "Timer.h"

class MovingPointStripPlugin : public StripPlugin
{
public:
	MovingPointStripPlugin();
	virtual void Loop() override;

private:
	Timer movingTimer;
	void onMovingEvent(TimerEventArgs& args);
	int ledCurrent = 0;
	enum MovingDirection { RIGTH, LEFT } movingDirection = RIGTH;;
};