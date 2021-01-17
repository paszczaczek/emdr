#pragma once
#include <Arduino.h>


struct Storage 
{
	
	enum struct MovingPointStripPlugin_Fun : unsigned long
	{
		SetSpeed = 0, SetHue, SetBrightness, EOE
	};
	MovingPointStripPlugin_Fun movingPointStripPlugin_Fun : 3;
	unsigned long movingPointStripPlugin_Hue : 8;
	unsigned long movingPointStripPlugin_Brightness : 8;
	unsigned long movingPointStripPlugin_Speed : 8;

};


extern Storage storage;