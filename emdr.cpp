#define FASTLED_INTERNAL
#include <Arduino.h>
#include <FastLED.h>
#include "Strip.h"
#include "MovingPointStripPlugin.h"

Strip emdrStrip;
Strip remoteControllStrip;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.write("setup");
	
	emdrStrip.SetController<WS2811, 7, GRB>(180);
	emdrStrip.AddPlugin(new MovingPointStripPlugin);

	remoteControllStrip.SetController<WS2812B, 8, GRB>(3);
	remoteControllStrip.ledsController->showColor(CRGB::Red);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	emdrStrip.Loop();
	remoteControllStrip.Loop();
}
