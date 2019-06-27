#include <sys/timeb.h>

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