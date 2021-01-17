#pragma once
#include <Arduino.h>


struct Storage 
{
	
	enum struct MovingPointStripPlugin_Fun : unsigned long
	{
		SetHue = 0, SetSpeed, SetBrightness, EOE
	};
	MovingPointStripPlugin_Fun movingPointStripPlugin_Fun : 3;
	unsigned long movingPointStripPlugin_Hue : 8;

};


extern Storage storage;