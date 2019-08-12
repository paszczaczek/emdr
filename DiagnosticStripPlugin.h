#pragma once
#include "Strip.h"
#include "StripPlugin.h"
#include "Timer.h"
#include "RemoteController.h"

class DiagnosticStipPlugin : public StripPlugin
{
public:
	DiagnosticStipPlugin()
	{
		timer.elapsed += timerElapsedEventHandler.Set(
			this, &DiagnosticStipPlugin::onTimerElapsed);
	}
	
	virtual void Loop() override
	{
		if (state == State::Stopped)
			return;
		timer.Loop();
	}

	// zapala na chwile wszystkie diody po wlaczeniu zasilania w celach diagnostycznych
	void PowerOn() {
		state = State::Started;
		timer.interval = 2000;
		timer.Start();
		PowerOn(0);
	}

private:
	Timer timer;
	EventHandler<DiagnosticStipPlugin, Timer::EventArgs> timerElapsedEventHandler;
	enum class Function { PowerOn, None } function;

	void onTimerElapsed(Timer::EventArgs& args)
	{
		byte callNo = args.elapsedIntervalsFromStart - args.elapsedIntervals + 1;
		switch (function)
		{
		case Function::PowerOn: PowerOn(callNo); break;
		case Function::None: break;
		}
	}

	void PowerOn(byte callNo)
	{
		CRGB color;
		switch (callNo)
		{
		case 0: // zaswiec wszystkie diody na bialo      
			Serial.print(F("PowerOn ..."));
			strip.SetLeds(CRGB::White);
			break;
		default:
			// zgas wszystkie diody - procedura zakonczona
			strip.SetLeds(CRGB::Black);
			timer.Stop();
			function = Function::None;
			Serial.print(F(" done"));
			strip.StartAllPlugins();
			break;
		}
		strip.updated = true;
	}
};
