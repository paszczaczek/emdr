#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "StripPlugin.h"
//#include "RemoteController.h"

class Strip
{
  public:
    // Strip(CRGB *leds, uint8_t ledsCount, RemoteController &remoteController, StripPlugin **plugins) :
    Strip(CRGB *leds, byte ledsCount, StripPlugin **plugins, byte pluginsCount) :
      leds(leds),
      ledsCount(ledsCount),
      plugins(plugins),
      pluginsCount(pluginsCount)
    { }

    template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
    void SetController()
    {
      pinMode(DATA_PIN, OUTPUT);
      controller = &FastLED.addLeds<CHIPSET, DATA_PIN, RGB_ORDER>(leds, ledsCount);
      FastLED.clear();
    }

    void StartAllPlugins()
    {      
      for (byte i = 0; i < pluginsCount; i++)
        plugins[i]->OnStart();
    }

    void Loop()
    {
      for (byte i = 0; i < pluginsCount; i++)
        plugins[i]->Loop();

      if (updated)
      {
        // controller->showLeds() nie uwzglednia FastLED.setMaxPowerInVoltsAndMilliamps() !
        //controller->showLeds(2);
        FastLED.show(2);
        updated = false;
      }
   }

    bool updated = false;
    CLEDController *controller = nullptr;

  private:
    CRGB *leds;
    byte ledsCount;
    StripPlugin **plugins;
    byte pluginsCount;
};
