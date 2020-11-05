float PB_maxValue;
uint8_t PB_row;
char PB_buffer[21];

// ---------------------------------------------------
void PB_init(uint8_t row, float maxValue) {
  PB_maxValue = maxValue;
  PB_row = row;
  PB_buffer[20] = 0;
}

// ---------------------------------------------------
void PB_set(float value){
  uint8_t numCols = round(value/PB_maxValue * 20.0f);
  for (int i = 0; i<20; i++){
    PB_buffer[i] = i < numCols ? '|' : ' '; 
  }
  lcd.setCursor(0,PB_row);
  lcd.print(PB_buffer);
}
