#define FASTLED_INTERNAL
#include <Arduino.h>
#include <FastLED.h>
#include "Strip.h"
#include "MovingPointStripPlugin.h"
#include "RemoteController.h"
#include "RemoteControllerStripPlugin.h"

#define MAX_CURRENT_FROM_EXT 1000 // Total maximum current draw when powered via external power supply
#define MAX_CURRENT_FROM_USB  500 // Total maximum current draw from the Arduino when powered from a USB port
#define MAX_CURRENT           100 // For safety

#define LED_TYPE WS2811
#define LED_PIN       7
#define LED_PIN_RC    8
#define LED_ORDER   GRB
//#define LED_COUNT	180 
#define LED_COUNT	  3 

Strip emdrStrip;
RemoteController remoteController;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
	Serial.write("setup\n");
	
	FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_CURRENT);
	emdrStrip.SetController<LED_TYPE, LED_PIN, LED_ORDER>(LED_COUNT);
	emdrStrip.AddPlugin(new MovingPointStripPlugin);
	emdrStrip.AddPlugin(new RemoteControllerStripPlugin<LED_TYPE, LED_PIN_RC, LED_ORDER>);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	emdrStrip.Loop();
}
