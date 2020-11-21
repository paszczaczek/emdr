#include "EmdrController.h"
#include "RemoteController.h"

RemoteController remoteController;

void SETUP()
{
	Serial.begin(115200);
	remoteController.Setup();
}

void LOOP()
{
	remoteController.Loop();

}
