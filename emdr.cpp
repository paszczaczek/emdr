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

Strip emdrStrip;
Strip remoteControllerStrip;
RemoteController remoteController;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
	Serial.write("setup\n");
	
	FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_CURRENT);
	emdrStrip.SetController<WS2811, 7, GRB>(/*3*/180);
	emdrStrip.AddPlugin(new MovingPointStripPlugin);

	//remoteControllerStrip.SetController<WS2812B, 8, GRB>(3);
	//remoteControllerStrip.AddPlugin(new RemoteControllerStripPlugin);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	emdrStrip.Loop();
	//remoteControllerStrip.Loop();
}
