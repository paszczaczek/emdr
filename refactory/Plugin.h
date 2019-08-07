#pragma once
#include <stdint.h>

class Plugin
{
public:
	virtual void Loop() = 0;

	virtual void OnStart()  { state = Started; }
	virtual void OnStop()   { state = Stopped; }
	virtual void OnPause()  { state = Paused; }
	virtual void OnResume() { state = Started; }
	enum State { Started, Stopped, Paused } state = Stopped;

	virtual ~Plugin() {};
};
