#include <Wire.h>
#include "EmdrController.h"
#include "RemoteController.h"

//constexpr int emdrAddr = 9;
RemoteController remoteController;

void SETUP()
{
	Serial.begin(115200);
	remoteController.Setup();

	Wire.begin();	
}

void LOOP()
{
	remoteController.Loop();
}
