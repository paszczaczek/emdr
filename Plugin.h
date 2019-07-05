#pragma once

class Plugin
{
public:
	virtual ~Plugin() {}
	bool isEnabled = true;
	virtual void Loop() = 0;
};
