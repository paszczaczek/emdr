#include <Wire.h>
#include <LCD_I2C.h>
#include "EmdrController.h"
#include "RemoteController.h"

constexpr int emdrAddr = 9;
RemoteController remoteController;

// https://github.com/blackhack/LCD_I2C
LCD_I2C lcd(0x27); // Default address of most PCF8574 modules, change according
unsigned long int b = 0;
unsigned long int skip = 0;
unsigned long int cnt = 0;

void SETUP()
{
	Serial.begin(115200);
	remoteController.Setup();
  
  Wire.begin();
 
	lcd.begin();
	lcd.backlight();
	lcd.print(" o  GOLE   ^o^");
	lcd.setCursor(0, 1);
	lcd.print("*|* BABY (*)|(*)");
  lcd.clear();
}

void LOOP()
{
	remoteController.Loop();

  const unsigned long int skip_max = 1e5;
  if (skip++ >= skip_max) {    
    Serial.println(cnt);
    lcd.setCursor(0, 0);
    lcd.print(cnt);
    skip = 0;
    cnt++;
    
    Wire.beginTransmission(emdrAddr);
    Wire.write(b++);
    Wire.endTransmission();
  }
}
