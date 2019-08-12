#pragma once
#include "Strip.h"
#include "StripPlugin.h"
#include "Timer.h"
#include "RemoteController.h"
#include "Emdr.h"

class MovingPointStripPlugin : public StripPlugin
{
  public:
	MovingPointStripPlugin() : movingTimer(2)
	{
	  elapsedEventHandler.Set(this, &MovingPointStripPlugin::onMovingTimerElapsed);      
	  movingTimer.elapsed += elapsedEventHandler;     
	  movingTimer.interval = 1000;      
	  
	  buttonPressedEventHandler.Set(this, &MovingPointStripPlugin::OnRemoteControllerButtonPressed);
	  remoteController.buttonPressed += buttonPressedEventHandler;
	}

	virtual void Loop() override
	{
	  //Serial.println("MovingPointStripPlugin::Loop");
	  movingTimer.Loop();
	}

	virtual void OnStart() override
	{
	  Plugin::OnStart();
	  movingTimer.Start();
	}

	virtual void OnPause() override
	{
	  Plugin::OnPause();
	  movingTimer.Stop();
	}

	virtual void OnResume() override
	{
	  Plugin::OnResume();
	  movingTimer.Start();
	}

  private:
	Timer movingTimer;
	int ledCurrent = 0;
	enum MovingDirection { RIGTH, LEFT } movingDirection = RIGTH;
	EventHandler<MovingPointStripPlugin, Timer::EventArgs> elapsedEventHandler;
	EventHandler<MovingPointStripPlugin, RemoteController::EventArgs> buttonPressedEventHandler;

	void onMovingTimerElapsed(Timer::EventArgs& args)
	{
	  //Serial.println(F("onMovingTimerElapsed"));
	  
	  auto ledNext = ledCurrent;

	  if (movingDirection == MovingDirection::RIGTH)
		  //if (ledCurrent < strip->controller->size() - (int)args.elapsedIntervals)
		  if (ledCurrent < strip.controller->size() - (int)args.elapsedIntervals)
		  ledNext += args.elapsedIntervals;
		else
		  movingDirection = MovingDirection::LEFT;
	  else if (ledCurrent > (int)args.elapsedIntervals - 1)
		ledNext -= args.elapsedIntervals;
	  else
		movingDirection = MovingDirection::RIGTH;

	  if (ledNext == ledCurrent)
		return;

	  strip.controller->leds()[ledCurrent] = CRGB::Black;
	  strip.controller->leds()[ledNext] = CRGB::Orange;
	  strip.updated = true;
	  //Serial.println("MovingPointStripPlugin::onTimerElapsed");

	  ledCurrent = ledNext;
	}

	void OnRemoteControllerButtonPressed(RemoteController::EventArgs &args)
	{
	  switch (args.button)
	  {
		case RemoteController::Button::CHANEL_PLUS:
		  if (movingTimer.interval > 20)
			movingTimer.interval /= 2;
		  break;
		case RemoteController::Button::CHANEL_MINUS:
		  movingTimer.interval *= 2;
		  break;
		case RemoteController::Button::PLAY:
		  movingTimer.Start();
		  break;
		case RemoteController::Button::PAUSE:
		  movingTimer.Stop();
		  break;
		default:
		  break;
	  }
	}
};
