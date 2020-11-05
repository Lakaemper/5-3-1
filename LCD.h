#include <Wire.h> 
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);


void LCD_init(){
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setBacklight(HIGH);
}
