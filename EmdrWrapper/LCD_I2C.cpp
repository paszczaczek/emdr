#include "LCD_I2C.h"
#include "EmdrWrapper.h"

void LCD_I2C::setCursor(uint8_t col, uint8_t row)
{
    EmdrWrapper::EmdrSketch::lcd::OnSetCursor(col, row);
}

void LCD_I2C::backlight()
{
    EmdrWrapper::EmdrSketch::lcd::OnBacklight(true);
}

void LCD_I2C::noBacklight()
{
    EmdrWrapper::EmdrSketch::lcd::OnBacklight(false);
}

void LCD_I2C::clear()
{
    EmdrWrapper::EmdrSketch::lcd::OnClear();
}
