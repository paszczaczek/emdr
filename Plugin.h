#pragma once

class Plugin
{
public:
	bool isEnabled = true;

public:
	virtual void Loop() = 0;
};
