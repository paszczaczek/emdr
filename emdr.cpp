#include <Arduino.h>
#include <FastLED.h>
#include "Strip.h"
#include "Emdr.h"

Strip emdrStrip;
Strip remoteControlStrip;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.write("setup");
	emdrStrip.SetController<WS2811, 7, GRB>(180);

	remoteControlStrip.SetController<WS2812B, 8, GRB>(1);
	remoteControlStrip.ledsController->showColor(CRGB::Blue);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	emdrStrip.Loop();
	remoteControlStrip.Loop();
}
