//#define FASTLED_ALLOW_INTERRUPTS 1
#define FASTLED_INTERNAL
#include <Arduino.h>
#include <FastLED.h>
#include <IRremote.h>
#include "Debug.h"
#include "MemoryFree.h"
#include "Timer.h"
#include "EventHandler.h"
#include "Strip.h"
#include "MovingPointStripPlugin.h"
#include "RemoteController.h"
#include "RemoteControllerStripPlugin.h"
#include "DiagnosticStripPlugin.h"

#define MAX_CURRENT_FROM_EXT 1000 // Total maximum current draw when powered via external power supply
#define MAX_CURRENT_FROM_USB  500 // Total maximum current draw from the Arduino when powered from a USB port

#define STRIP_LEDS_TYPE  WS2812B // led controller type
#define STRIP_LEDS_PIN         7 // pin for main strip
#define STRIP_LEDS_PIN_RC      8 // pin for remote control strip
#define STRIP_LEDS_ORDER     GRB
#define STRIP_LEDS_COUNT     30 //120
#define STRIP_LEDS_BRIGHTNESS  2
#define STRIP_LEDS_MAX_CURRENT MAX_CURRENT_FROM_USB

#define RC_PIN 2 // pin for remote controller

// remote controller
IRrecv irrecv(RC_PIN);
decode_results irrecvResults;
RemoteController remoteController(&irrecv, &irrecvResults);

// strip plugins
DiagnosticStipPlugin diagnosticStipPlugin;
MovingPointStripPlugin movingPointStripPlugin;
RemoteControllerStripPlugin remoteControllerstripPlugin;
const byte stripPluginsCount = 3;
StripPlugin* stripPlugins[stripPluginsCount] = {
  &diagnosticStipPlugin,
  &movingPointStripPlugin,
  &remoteControllerstripPlugin
};

// strip
CRGB stripLeds[STRIP_LEDS_COUNT];
Strip strip(
	stripLeds, STRIP_LEDS_COUNT,
	stripPlugins, stripPluginsCount,
	STRIP_LEDS_MAX_CURRENT, STRIP_LEDS_BRIGHTNESS);

void setup()
{
	Serial.begin(115200);
	PRINT_FREEMEM(F("setup"));

	irrecv.enableIRIn();
	strip.SetController<STRIP_LEDS_TYPE, STRIP_LEDS_PIN, STRIP_LEDS_ORDER>();
	diagnosticStipPlugin.PowerOn();
}

void loop() {
	remoteController.Loop();
	strip.Loop();
}
