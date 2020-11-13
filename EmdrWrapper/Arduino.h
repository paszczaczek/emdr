#pragma once
#include <inttypes.h>
#include "HardwareSerial.h"

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

typedef unsigned char byte;
extern const char *F(const char *str);

extern unsigned long millis(void);
extern void delay(unsigned long);
extern void pinMode(uint8_t, uint8_t);