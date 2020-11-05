uint8_t SC_row0, SC_row1, SC_width;
int8_t SC_col;
char SC_selBuffer[10];
char SC_eraseBuffer[10];


// -----------------------------------------------------------
void SC_init(uint8_t r0, uint8_t r1, uint8_t w) {
  SC_row0 = r0;
  SC_row1 = r1;
  SC_width = w;
  SC_col = -1;
  for (int i = 0; i < w; i++) {
    SC_selBuffer[i] = '-';
    SC_eraseBuffer[i] = ' ';
  }
  SC_selBuffer[w] = 0;
  SC_eraseBuffer[w] = 0;
}

// -----------------------------------------------------------
void SC_set(int col) {
  if (SC_col != -1) {
    lcd.setCursor(SC_col, SC_row0);
    lcd.print(SC_eraseBuffer);
    if (SC_row1 > SC_row0) {
      lcd.setCursor(SC_col, SC_row1);
      lcd.print(SC_eraseBuffer);
    }
  }
  SC_col = col;
  lcd.setCursor(SC_col, SC_row0);
  lcd.print(SC_selBuffer);
  if (SC_row1 > SC_row0) {
    lcd.setCursor(SC_col, SC_row1);
    lcd.print(SC_selBuffer);
  }
}


// -----------------------------------------------------------
void SC_clear() {
  if (SC_col != -1) {
    lcd.setCursor(SC_col, SC_row0);
    lcd.print(SC_eraseBuffer);
    lcd.setCursor(SC_col, SC_row1);
    lcd.print(SC_eraseBuffer);
    SC_col = -1;
  }
}
