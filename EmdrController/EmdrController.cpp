#include <LCD_I2C.h>
#include "EmdrController.h"
#include "RemoteController.h"

RemoteController remoteController;

// https://github.com/blackhack/LCD_I2C
LCD_I2C lcd(0x27); // Default address of most PCF8574 modules, change according

void SETUP()
{
	Serial.begin(115200);
	remoteController.Setup();
	lcd.begin();
	lcd.backlight();
	lcd.print(" o  GOLE   ^o^");
	lcd.setCursor(0, 1);
	lcd.print("*|* BABY (*)|(*)");
}

void LOOP()
{
	remoteController.Loop();
}
