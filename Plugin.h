#pragma once

class Plugin
{
public:
	bool isEnabled = true;

	virtual void Loop() = 0;
	virtual ~Plugin() {};
};
