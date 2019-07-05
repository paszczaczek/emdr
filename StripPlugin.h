#pragma once
#include "Plugin.h"

class Strip;

class StripPlugin : public Plugin
{
public:
	virtual ~StripPlugin() override {}
	Strip *strip = nullptr;
};
