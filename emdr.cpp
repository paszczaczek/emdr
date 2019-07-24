#define FASTLED_INTERNAL
#include <Arduino.h>
#include <FastLED.h>
#include <IRremote.h>
#include "Strip.h"
#include "MovingPointStripPlugin.h"
#include "RemoteController.h"
#include "RemoteControllerStripPlugin.h"
#include "PowerStripPlugin.h"

#define MAX_CURRENT_FROM_EXT 1000 // Total maximum current draw when powered via external power supply
#define MAX_CURRENT_FROM_USB  500 // Total maximum current draw from the Arduino when powered from a USB port
#define MAX_CURRENT           100 // For safety

#define LED_TYPE WS2811
#define LED_PIN       7 // pin for main strip
#define LED_PIN_RC    8 // pin for remote control strip
#define LED_ORDER   GRB
//#define LED_COUNT	180 
#define LED_COUNT	  3 

#define RC_PIN 2        // pin dor remote controller

IRrecv irrecv(RC_PIN);
RemoteController remoteController(irrecv);
Strip strip(remoteController);

void setup()
{
	Serial.begin(115200);
	Serial.println("setup");
	
	FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_CURRENT);
	strip.SetController<LED_TYPE, LED_PIN, LED_ORDER>(LED_COUNT);
	strip.AddPlugin(new MovingPointStripPlugin);
	strip.AddPlugin(new RemoteControllerStripPlugin<LED_TYPE, LED_PIN_RC, LED_ORDER>);
	strip.AddPlugin(new PowerStipPlugin);

	Serial.println("enableIRIn");
	irrecv.enableIRIn();
}

void loop() {	
	remoteController.Loop();
	strip.Loop();
}
