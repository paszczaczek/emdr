#pragma once
#include <sys/timeb.h>
#include "HardwareSerial.h"

unsigned long millis(void)
{
	static struct timeb boot = { 0, 0, 0, 0 };
	if (boot.time == 0)
		ftime(&boot);

	struct timeb end;
	ftime(&end);
	unsigned long ms
		= (unsigned int)(end.time - boot.time) * 1000
		+ (end.millitm - boot.millitm);

	return ms;
}