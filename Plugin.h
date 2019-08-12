#pragma once
#include <stdint.h>
#include "Arduino.h"

class Plugin
{
public:
	virtual void Loop() = 0;

	virtual void OnStart()  { state = State::Started; }
	virtual void OnStop()   { state = State::Stopped; }
	virtual void OnPause()  { state = State::Paused; }
	virtual void OnResume() { state = State::Started; }
	enum struct State :byte { Started, Stopped, Paused } state = State::Stopped;

	virtual ~Plugin() {};
};
