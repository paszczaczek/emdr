#pragma once
#include <string.h>

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

class HardwareSerial //: public Stream
{
public:
	void begin(unsigned long baud) {}
	size_t write(const char *str);
	size_t print(const char *str);
	size_t println(const char *str);
	size_t print(int value, int format = DEC);
	size_t println(int value, int format = DEC);
	size_t println();
};
extern HardwareSerial Serial;