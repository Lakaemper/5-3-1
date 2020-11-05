uint8_t TB_row;
uint8_t TB_col;
unsigned long TB_timeInterval;
uint8_t TB_state;
Timer TB_timer;

// ----------------------------------------------------------------
void TB_init() {
  TB_state = 0;
}

// ----------------------------------------------------------------
void TB_set(unsigned long timeS, int row) {
  TB_row = row;
  TB_col = 20;
  TB_timeInterval = timeS * 1000 / 20;
  TB_timer.set(TB_timeInterval, true);
  TB_state = 1;
  for (int i = 0; i < TB_col; i++) {
    lcd.setCursor(i, TB_row);
    lcd.print("|");
  }
}

// ----------------------------------------------------------------
uint8_t TB_process() {
  uint8_t resultEvent = 0;
  if (TB_state != 0) {
    if (TB_timer.process()) {
      TB_col--;
      lcd.setCursor(TB_col, TB_row);
      lcd.print(" ");
      if (TB_col == 0) {
        TB_timer.stop();
        resultEvent = 1;
      }
    }
  }
  return resultEvent;
}
