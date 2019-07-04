#include <Arduino.h>
#include <stdio.h>
#include "Strip.h"
#include "Emdr.h"

Strip strip;
Strip remoteControlStrip;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.write("setup");
}

// the loop function runs over and over again until power down or reset
void loop()
{
	//#define _CRT_SECURE_NO_WARNINGS
	//char buf[256] = "";
	//sprintf(buf, "loop %.3fs", millis()/1000.0);
	//Serial.write(buf);
	strip.Loop();
  Serial.write("loop");
}
