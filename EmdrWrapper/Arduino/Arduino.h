#pragma once
#include <inttypes.h>
#include "HardwareSerial.h"
#include "Print.h"

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define LED_BUILTIN 13

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define F(string_literal) string_literal
typedef char __FlashStringHelper;

typedef uint8_t byte;

extern unsigned long millis(void);
extern void delay(unsigned long);
extern void pinMode(uint8_t, uint8_t);
extern int digitalRead(uint8_t pin);
extern void digitalWrite(uint8_t pin, uint8_t val);

