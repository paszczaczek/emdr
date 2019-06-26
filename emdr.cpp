#include <Arduino.h>
#include "Strip.h"
#include "Emdr.h"

Strip strip;

// the setup function runs once when you press reset or power the board
void setup() 
{
	Serial.write("setup");
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	Serial.write("loop");
	strip.Loop();
}
