#include "EmdrController.h"
#include "RemoteController.h"

constexpr int RC_PIN = 2; // pin for remote controller

// remote controller
IRrecv irrecv(RC_PIN);
RemoteController remoteController;

void SETUP()
{
	irrecv.enableIRIn();
}

void LOOP()
{
	remoteController.Loop();
}