#define FASTLED_INTERNAL
#include <Arduino.h>
#include <FastLED.h>
#include "Timer.h"
#include "EventHandler.h"
//#include <IRremote.h>
//#include "MemoryFree.h"
//#include "Debug.h"
//#include "Strip.h"
#include "MovingPointStripPlugin.h"
//#include "RemoteController.h"
//#include "RemoteControllerStripPlugin.h"
//#include "PowerStripPlugin.h"

#define MAX_CURRENT_FROM_EXT 1000 // Total maximum current draw when powered via external power supply
#define MAX_CURRENT_FROM_USB  500 // Total maximum current draw from the Arduino when powered from a USB port
#define MAX_CURRENT           100 // For safety

//#define LED_TYPE WS2811
#define LED_TYPE WS2812B
#define LED_PIN       7 // pin for main strip
#define LED_PIN_RC    8 // pin for remote control strip
#define LED_ORDER   GRB
#define LED_COUNT   180

#define RC_PIN 2        // pin dor remote controller

//IRrecv irrecv(RC_PIN);
//decode_results irrecvResults;
//RemoteController remoteController(&irrecv, &irrecvResults);

MovingPointStripPlugin movingPointStripPlugin;

/*
StripPlugin* stripPlugins[] = {
  &movingPointStripPlugin
};
CRGB leds[LED_COUNT];
Strip strip(leds, LED_COUNT, stripPlugins, 1);
*/


class Test {
public:
  void Callback(Timer::EventArgs& args) {
    Serial.print("      Callback: ");
    Serial.print(args.elapsedIntervals);
    Serial.println();
  }
};

Timer timer;
EventHandler<Test, Timer::EventArgs> handler;
Test test;


void setup()
{
  Serial.begin(115200);
  Serial.println("setup");

  /*
  handler.Set(&test, &Test::Callback);
  timer.elapsed += handler;
  timer.interval = 1000;
  timer.Start();
  */
  
  //PRINT_FREEMEM(F("setup"));
  //irrecv.enableIRIn();
  /*
    strip.SetController<LED_TYPE, LED_PIN, LED_ORDER>();
    FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_CURRENT);
    strip.StartAllPlugins();
  */
  movingPointStripPlugin.OnStart();
  Serial.println("setup end");
}

void loop() {
  /*
  timer.Loop();
  */
  //remoteController.Loop();
  movingPointStripPlugin.Loop();
}
