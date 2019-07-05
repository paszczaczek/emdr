#define FASTLED_INTERNAL
#include <Arduino.h>
#include <FastLED.h>
#include "Strip.h"
#include "MovingPointStripPlugin.h"
#include "RemoteController.h"
#include "RemoteControllerStripPlugin.h"

Strip emdrStrip;
Strip remoteControllerStrip;
RemoteController remoteController;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.write("setup");
	
	emdrStrip.SetController<WS2811, 7, GRB>(180);
	emdrStrip.AddPlugin(new MovingPointStripPlugin);

	remoteControllerStrip.SetController<WS2812B, 8, GRB>(3);
	remoteControllerStrip.AddPlugin(new RemoteControllerStripPlugin);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	emdrStrip.Loop();
	remoteControllerStrip.Loop();
}
