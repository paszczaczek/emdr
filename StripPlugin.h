#pragma once
#include "Plugin.h"

class Strip;

class StripPlugin : public Plugin
{
public:
	Strip *strip = nullptr;
};
