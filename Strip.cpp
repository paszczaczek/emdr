#include <FastLED.h>
#include "Strip.h"
#include "StripPlugin.h"

Strip::~Strip()
{
	for (auto plugin : plugins)
		delete plugin;

	delete[] leds;
}

void Strip::AddPlugin(StripPlugin *plugin)
{
	plugin->strip = this;
	plugins.push_back(plugin);
}

void Strip::Loop()
{
	for (auto plugin : plugins)
		plugin->Loop();

	if (updated)
	{
		// controller->showLeds() nie uwzglednia FastLED.setMaxPowerInVoltsAndMilliamps() !
		//controller->showLeds(2);
		FastLED.show(2);
		updated = false;
	}
}