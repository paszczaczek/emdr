#include "Event.h"
#include "Device.h"

void Event::Send(Name name)
{
	stripDevice.Receive(name);
}
