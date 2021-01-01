#pragma once
#include "EmdrController/Event.h"

class Plugin
{
public:
	enum struct State : byte { Started, Stopped, Paused } state = State::Stopped;

	virtual ~Plugin() {};

	virtual void Loop() = 0;
	virtual void Start()  { state = State::Started; }
	virtual void Stop()   { state = State::Stopped; }
	virtual void Pause()  { state = State::Paused; }

	virtual bool Receive(Event::Name eventReceived) = 0;

};

// sizeof(Plugin) = 3
