/*
 * 5 - 3 - 1 Drum Timer
 * Using 20x4 LCD and Rotary Knob
 * Written for Arduino Nano (Atmega 328P old bootloader)
 */

#include "EEPROM.h" 
#include "Globals.h"
#include "Rotary.h"
#include "LCD.h"
#include "SelectionCursor.h"
#include "ProgressBar.h"
#include "TimerClass.h";
#include "TimeoutBar.h";
#include "StateMachine.h"

void setup()
{
  Serial.begin(9600);
  EE_init();
  TB_init();
  RT_init();
  LCD_init();
  SM_init();
}


void loop()
{
  uint8_t event = RT_process();
  //
  // process
  SM_process(event);
}
