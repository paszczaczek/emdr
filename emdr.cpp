#include "Arduino.h"

// the setup function runs once when you press reset or power the board
void setup() 
{
	Serial.write("setup");
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	Serial.write("loop");
}
