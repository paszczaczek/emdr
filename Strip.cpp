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
}