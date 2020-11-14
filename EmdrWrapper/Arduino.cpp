#include <sys/timeb.h>
#include <inttypes.h>
#include <windows.h>

unsigned long millis(void)
{
	static struct timeb boot = { 0, 0, 0, 0 };
	if (boot.time == 0)
		ftime(&boot);

	struct timeb end;
	ftime(&end);
	unsigned long ms = (unsigned int)(end.time - boot.time) * 1000;
	unsigned long msdiff = end.millitm - boot.millitm;
	if (msdiff >= 0)
	{
		ms += msdiff;
	}
	else
	{
		ms--;
		ms += 1000 + msdiff;
	}

	return ms;
}

void delay(unsigned long ms)
{
	Sleep(ms);
}

int digitalRead(uint8_t pin)
{
	return 0x0;
}


void pinMode(uint8_t, uint8_t) 
{
}

const char *F(const char *str) 
{ 
	return str; 
}