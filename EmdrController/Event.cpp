#include "Event.h"

#ifdef _WIN32
#include "EmdrWrapper.h"
#endif

void Event::Send(Event::Name name)
{
#ifdef _WIN32
	EmdrWrapper::EmdrControllerSketch::Event::OnSend((int)name);
#else
#endif
}