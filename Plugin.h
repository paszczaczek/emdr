#pragma once
#include "Arduino.h"
#include "RemoteController.h"

class Plugin
{
public:
	enum struct State :byte { Started, Stopped, Paused } state = State::Stopped;

	virtual ~Plugin() {};

	virtual void Loop() = 0;
	virtual void OnStart()  { state = State::Started; }
	virtual void OnStop()   { state = State::Stopped; }
	virtual void OnPause()  { state = State::Paused; }
	virtual void OnResume() { state = State::Started; }
	virtual void OnRemoteControllerEvent(RemoteController::EventArgs &) { }

};
