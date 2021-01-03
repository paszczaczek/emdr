#pragma once
#include <LCD_I2C.h>
#include <IRremote.h>

// EmdrWrapper linkuje Emdr.cpp i EmdrController.cpp a w obu s¹ funkcje setup i loop
// trzeba im nadaæ rózne nazwy
#ifdef _WIN32
#define SETUP setup_controller
#define LOOP loop_controller
#else
#define SETUP setup
#define LOOP loop
#endif


extern void SETUP();
extern void LOOP();

extern IRrecv irrecv;
