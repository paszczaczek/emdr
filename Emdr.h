#pragma once
#include <LCD_I2C.h>


//extern RemoteController remoteController;
extern void setup();
extern void loop();
extern void onReceiveFromController(int);
extern bool isDevelMode();

