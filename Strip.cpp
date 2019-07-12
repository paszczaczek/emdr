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
		// z kolei FastLED.show() nie dziala, jak jest dodany drugi kontroler dla pilota 
		FastLED.show(2);
		//FastLED[2].showLeds(3);
		//FastLED[1].showLeds(3);
		//controller->showLeds(2);
		updated = false;
	}
}