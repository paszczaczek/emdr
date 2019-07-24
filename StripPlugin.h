#pragma once
#include "Plugin.h"

class Strip;

class StripPlugin : public Plugin
{
public:
	virtual void OnSetStrip(Strip *strip) = 0;
	virtual ~StripPlugin() override {};
	Strip *strip = nullptr;
};
