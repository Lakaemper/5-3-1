
#define SM_STATE_WELCOME_SCREEN 0
#define SM_STATE_SELECTION_SCREEN 1

#define SM_STATE_TIMER_SCREEN 2
#define SM_STATE_TIMER_SETTIME 3
#define SM_STATE_TIMER_RUN 4
#define SM_STATE_TIMER_FINISHED 5

#define SM_STATE_METRO531_SCREEN 6
#define SM_STATE_METRO531_TUNEBPM 7
#define SM_STATE_METRO531_SETSPEED 8
#define SM_STATE_METRO531_GET_READY 9
#define SM_STATE_METRO531_RUN 10
#define SM_STATE_METRO531_FINISHED 11

#define SM_BACKLIGHT_FLASHTIME 100

uint8_t SM_stateIsInitialized;
uint8_t SM_state;
uint8_t SM_selection;
uint8_t SM_timerMinutes;
uint8_t SM_timerSeconds;
char SM_timeBuffer[6];
char SM_lineBuffer[21];
float SM_BPM[3];
uint8_t SM_time531;
Timer SM_timer0;
Timer SM_timerBPM;
bool SM_backlight;
unsigned long SM_backlightOffTime;
bool SM_531hasMetro;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SM_switchState(uint8_t state);
inline bool SM_hasEvent(uint8_t event, uint8_t mask);
void SM_initWelcomeScreen();
void SM_initSelectionScreen();
void SM_processSelectionScreen(uint8_t event);
void SM_initTimerScreen();
void SM_processTimerScreen(uint8_t event);
void SM_initTimerSetTime();
void SM_processTimerSetTime(uint8_t event);
void SM_initTimerRun();
bool SM_processTimerRun();
void SM_initTimerFinished();
void SM_processTimerFinished(uint8_t event);
void SM_initMetro();
void SM_initMetroScreen();
void SM_processMetroScreen(uint8_t event);
void SM_initTuneScreen();
bool SM_processTuneScreen(uint8_t event);
void SM_initMetroSpeed();
void SM_processMetroSpeed(uint8_t event);
void SM_initMetroGetReady();
void SM_initMetroRun();
bool SM_processMetroRun();
void SM_initMetroFinished();
void SM_processMetroFinished(uint8_t event);

// ----------------------------------------------------------
void SM_init() {
  SM_switchState(SM_STATE_WELCOME_SCREEN);
}

// ----------------------------------------------------------
// STATE MACHINE MAIN ENTRY POINT
// ----------------------------------------------------------
void SM_process(uint8_t event) {
  //
  // longpress of button always returns to welcome screen
  if (SM_hasEvent(event, EVENT_BUTTON_LONG) && SM_state != SM_STATE_WELCOME_SCREEN) {
    SM_switchState(SM_STATE_WELCOME_SCREEN);
  }
  //

  switch (SM_state) {
    // --------------------------
    case SM_STATE_WELCOME_SCREEN: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initWelcomeScreen();
          SM_timer0.set(1500, false);
        }
        //
        if (SM_timer0.process()) {
          SM_timer0.stop();
          SM_switchState(SM_STATE_SELECTION_SCREEN);
        }
        break;
      }
    // --------------------------
    case SM_STATE_SELECTION_SCREEN: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initSelectionScreen();
          // set selection cursor one left and simulate turn right
          SM_selection = 1;
          event = EVENT_TURN_RIGHT_MULTI;
        }
        //
        SM_processSelectionScreen(event);
        if (SM_hasEvent(event, EVENT_BUTTON_SHORT)) {
          if (SM_selection == 0) SM_switchState(SM_STATE_TIMER_SCREEN);
          else if (SM_selection == 1) {
            SM_531hasMetro = true;
            SM_switchState(SM_STATE_METRO531_SCREEN);
          }
          else if (SM_selection == 2) {
            SM_531hasMetro = false;
            SM_switchState(SM_STATE_METRO531_SCREEN);
          }
        }
        //
        break;
      }
    // --------------------------
    case SM_STATE_TIMER_SCREEN: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initTimerScreen();
        }
        //
        SM_processTimerScreen(event);
        if (SM_hasEvent(event, EVENT_BUTTON_SHORT)) {
          if (SM_selection == 0) SM_switchState(SM_STATE_TIMER_SETTIME);
          if (SM_selection == 1) SM_switchState(SM_STATE_TIMER_RUN);
        }
        break;
      }
    // --------------------------
    case SM_STATE_TIMER_SETTIME: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initTimerSetTime();
        }
        SM_processTimerSetTime(event);
        if (SM_hasEvent(event, EVENT_BUTTON_SHORT)) {
          SM_switchState(SM_STATE_TIMER_RUN);
        }
        break;
      }
    // --------------------------
    case SM_STATE_TIMER_FINISHED: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initTimerFinished();
        }
        SM_processTimerFinished(event);
        if (SM_hasEvent(event, EVENT_BUTTON_SHORT)) {
          if (SM_selection == 0) SM_switchState(SM_STATE_TIMER_SETTIME);
          if (SM_selection == 1) SM_switchState(SM_STATE_TIMER_RUN);
          if (SM_selection == 2) SM_switchState(SM_STATE_SELECTION_SCREEN);
        }
        break;
      }
    // ----------------------------
    case SM_STATE_TIMER_RUN: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initTimerRun();
        }
        bool timesUp = SM_processTimerRun();
        if (timesUp) {
          SM_switchState(SM_STATE_TIMER_FINISHED);
        }
        break;
      }
    // --------------------------
    case SM_STATE_METRO531_SCREEN: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initMetro();
          // proceed to next state if there's no metronome
          if (!SM_531hasMetro) {
            SM_switchState(SM_STATE_METRO531_GET_READY);
            break;
          }
          //
          SM_initMetroScreen();
        }
        SM_processMetroScreen(event);
        if (SM_hasEvent(event, EVENT_BUTTON_SHORT)) {
          if (SM_selection == 0) SM_switchState(SM_STATE_METRO531_TUNEBPM);
          if (SM_selection == 1) SM_switchState(SM_STATE_METRO531_SETSPEED);
          if (SM_selection == 2) {
            SM_switchState(SM_STATE_METRO531_GET_READY);
          }
        }
        break;
      }

    // --------------------------
    case SM_STATE_METRO531_TUNEBPM: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initTuneScreen();
        }
        bool exitTune = SM_processTuneScreen(event);
        if (exitTune) {
          SM_switchState(SM_STATE_METRO531_SCREEN);
        }
        break;
      }

    // --------------------------
    case SM_STATE_METRO531_SETSPEED: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initMetroSpeed();
        }
        SM_processMetroSpeed(event);
        if (SM_hasEvent(event, EVENT_BUTTON_SHORT)) {
          SM_time531 = 5;
          SM_switchState(SM_STATE_METRO531_GET_READY);
        }
        break;
      }
    // --------------------------
    case SM_STATE_METRO531_GET_READY: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initMetroGetReady();
        }
        if (TB_process() || SM_hasEvent(event, EVENT_BUTTON_SHORT)) {
          SM_switchState(SM_STATE_METRO531_RUN);
        }
        break;
      }
    // --------------------------
    case SM_STATE_METRO531_RUN: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initMetroRun();
        }
        bool timeIsUp = SM_processMetroRun();
        if (timeIsUp) {
          lcd.setBacklight(HIGH);
          if (SM_time531 == 1) {
            SM_switchState(SM_STATE_METRO531_FINISHED);
          } else {
            SM_time531 -= 2;
            SM_switchState(SM_STATE_METRO531_GET_READY);
          }
        }
        break;
      }
    // --------------------------
    case SM_STATE_METRO531_FINISHED: {
        if (!SM_stateIsInitialized) {
          SM_stateIsInitialized = true;
          SM_initMetroFinished();
        }
        SM_processMetroFinished(event);
        if (SM_hasEvent(event, EVENT_BUTTON_SHORT)) {
          if (SM_selection == 0) SM_switchState(SM_STATE_METRO531_SETSPEED);
          if (SM_selection == 1) SM_switchState(SM_STATE_SELECTION_SCREEN);
        }
        break;
      }
  }
}
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// END STATE MACHINE MAIN
// ---------------------------------------------------------------------------------------------------

// ----------------------------------------------------------
void SM_switchState(uint8_t state) {
  SM_stateIsInitialized = false;
  SM_state = state;
  Serial.print("SM: switching to state ");
  Serial.println(state);
}

// ----------------------------------------------------------
inline bool SM_hasEvent(uint8_t event, uint8_t mask) {
  return event & mask;
}

// ----------------------------------------------------------
void SM_initWelcomeScreen() {
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("--------------------"));
  lcd.setCursor(4, 1);
  lcd.print(F("5' - 3' - 1'"));
  lcd.setCursor(5, 2);
  lcd.print(F("DRUM TIMER"));
  lcd.setCursor(0, 3);
  lcd.print(F("--------------------"));
}

// ----------------------------------------------------------
void SM_initSelectionScreen() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(F("PURE   5-3-1  5-3-1"));
  lcd.setCursor(0, 2);
  lcd.print(F("TIMER  METRO  TIMER"));
  SC_init(0, 3, 5);
}

// ----------------------------------------------------------
void SM_processSelectionScreen(uint8_t event) {
  if (!SM_hasEvent(event, EVENT_TURN_LEFT_MULTI | EVENT_TURN_RIGHT_MULTI)) {
    return;
  }
  if (SM_hasEvent(event, EVENT_TURN_LEFT_MULTI)) {
    SM_selection = SM_selection == 0 ? 2 : SM_selection - 1;
  }
  if (SM_hasEvent(event, EVENT_TURN_RIGHT_MULTI)) {
    SM_selection = (SM_selection + 1 ) % 3;
  }
  SC_set(SM_selection * 7);
}

// ----------------------------------------------------------
void SM_printTime(uint8_t mnts, uint8_t scnds, uint8_t c, uint8_t r) {
  lcd.setCursor(c, r);
  sprintf(SM_timeBuffer, "%02d'%02d", mnts, scnds);
  SM_timeBuffer[5] = 0;
  lcd.print(SM_timeBuffer);
}

// ----------------------------------------------------------
// print the 3 BPM values i designated row at columns 3, 10, 17
void SM_printBPM(uint8_t row) {
  SM_timeBuffer[3] = 0;
  for (int i = 0; i < 3; i++) {
    sprintf(SM_timeBuffer, "%03d", (int)round(SM_BPM[i]));
    SM_timeBuffer[3] = 0;
    lcd.setCursor(3 + i * 7, row);
    lcd.print(SM_timeBuffer);
  }
}

// ----------------------------------------------------------
void SM_eraseRow(uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print(F("                    "));
}

// ----------------------------------------------------------
void SM_initTimerScreen() {
  uint8_t ms[2];
  EE_readTimerData(ms);
  SM_timerMinutes = ms[0];
  SM_timerSeconds = ms[1];
  //
  lcd.clear();
  SM_printTime(SM_timerMinutes, SM_timerSeconds, 3, 1);
  lcd.setCursor(12, 1);
  lcd.print(F("START"));
  SM_selection = 0;
  SC_init(0, 2, 5);
  SM_processTimerScreen(EVENT_TURN_RIGHT_MULTI);
}

// ----------------------------------------------------------
void SM_processTimerScreen(uint8_t event) {
  if (SM_hasEvent(event, EVENT_TURN_LEFT_MULTI | EVENT_TURN_RIGHT_MULTI)) {
    SM_selection = SM_selection == 0 ? 1 : 0;
    SC_set(3 + SM_selection * 9);
  }
}

// ----------------------------------------------------------
void SM_initTimerSetTime() {
  uint8_t ms[2];
  EE_readTimerData(ms);
  SM_timerMinutes = ms[0];
  SM_timerSeconds = ms[1];
  //
  lcd.clear();
  lcd.setCursor(6, 1);
  lcd.print(F("SET TIME:"));
  SM_printTime(SM_timerMinutes, SM_timerSeconds, 8, 2);
  SC_init(0, 3, 9);
  SC_set(6);
}

// ----------------------------------------------------------
void SM_processTimerSetTime(uint8_t event) {
  uint8_t timeStepSec = 10;
  bool doPrint = false;
  // decrease time
  if (SM_hasEvent(event, EVENT_TURN_LEFT_SINGLE)) {
    if (SM_timerMinutes > 0 || SM_timerSeconds > 0) {
      if (SM_timerSeconds < timeStepSec) {
        if (SM_timerMinutes > 0) {
          SM_timerMinutes--;
        }
        SM_timerSeconds = 60;
      }
      SM_timerSeconds -= timeStepSec;
      doPrint = true;
    }
  }
  if (SM_hasEvent(event, EVENT_TURN_RIGHT_SINGLE)) {
    if (SM_timerMinutes < 99 || SM_timerSeconds < 60 - timeStepSec) {
      if (SM_timerSeconds >= 60 - timeStepSec) {
        if (SM_timerMinutes < 99) {
          SM_timerMinutes++;
        }
        SM_timerSeconds = 60 - timeStepSec;
      }
      SM_timerSeconds += timeStepSec;
      SM_timerSeconds %= 60;
      doPrint = true;
    }
  }
  if (SM_hasEvent(event, EVENT_BUTTON_SHORT)) {
    uint8_t ms[2];
    ms[0] = SM_timerMinutes;
    ms[1] = SM_timerSeconds;
    EE_writeTimerData(ms);
  }
  if (doPrint) {
    SM_printTime(SM_timerMinutes, SM_timerSeconds, 8, 2);
  }
}

// ----------------------------------------------------------
void SM_initTimerRun() {
  uint8_t ms[2];
  EE_readTimerData(ms);
  SM_timerMinutes = ms[0];
  SM_timerSeconds = ms[1];
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("REMAINING:");
  SM_printTime(SM_timerMinutes, SM_timerSeconds, 13, 1);
  PB_init(3, (float)SM_timerMinutes * 60.0f + (float)SM_timerSeconds);
  PB_set((float)SM_timerMinutes * 60.0f + (float)SM_timerSeconds);
  SM_timer0.set(1000, true);
}

// ----------------------------------------------------------
bool SM_processTimerRun() {
  if (SM_timerMinutes == 0 && SM_timerSeconds == 0) {
    SM_timer0.stop();
    return true;
  }
  bool timeIsUp = false;
  //
  if (SM_timer0.process()) {
    if (SM_timerSeconds == 0) {
      SM_timerSeconds = 60;
      SM_timerMinutes--;
    }
    SM_timerSeconds--;
    timeIsUp = !(SM_timerMinutes || SM_timerSeconds);
    if (timeIsUp) {
      SM_timer0.stop();
    }
    SM_printTime(SM_timerMinutes, SM_timerSeconds, 13, 1);
    PB_set((float)SM_timerMinutes * 60.0f + (float)SM_timerSeconds);
  }
  return timeIsUp;
}

// ----------------------------------------------------------
void SM_initTimerFinished() {
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("TIME'S UP!");
  uint8_t ms[2];
  EE_readTimerData(ms);
  SM_printTime(ms[0], ms[1], 1, 2);
  lcd.setCursor(7, 2);
  lcd.print(F("REPEAT"));
  lcd.setCursor(15, 2);
  lcd.print(F("EXIT"));
  SM_selection = 1;
  SC_init(1, 3, 6);
  SC_set(SM_selection * 7);
}

// ----------------------------------------------------------
void SM_processTimerFinished(uint8_t event) {
  if (!SM_hasEvent(event, EVENT_TURN_LEFT_MULTI | EVENT_TURN_RIGHT_MULTI)) {
    return;
  }
  if (SM_hasEvent(event, EVENT_TURN_LEFT_MULTI)) {
    SM_selection = SM_selection == 0 ? 2 : SM_selection - 1;
  }
  if (SM_hasEvent(event, EVENT_TURN_RIGHT_MULTI)) {
    SM_selection = (SM_selection + 1 ) % 3;
  }
  SC_set(SM_selection * 7);
}

// ----------------------------------------------------------
void SM_initMetro() {
  uint8_t bpm[3];
  EE_readBPM(bpm);
  SM_BPM[0] = (float)bpm[0];
  SM_BPM[1] = (float)bpm[1];
  SM_BPM[2] = (float)bpm[2];
  SM_time531 = 5;
}

// ----------------------------------------------------------
void SM_initMetroScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("5':    3':    1':"));
  SM_printBPM(0);
  lcd.setCursor(0, 2);
  lcd.print(F("TUNE   SPEED  START"));
  SC_init(1, 3, 5);
  SM_selection = 2;
  SC_set(14);
}

// ----------------------------------------------------------
void SM_initTuneScreen() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(F("5':    3':    1':"));
  SM_printBPM(1); 
  SC_init(0, 2, 6);
  SC_set(0);
  SM_selection = 0;
}

// ----------------------------------------------------------
bool SM_processTuneScreen(uint8_t event) {
  if (!SM_hasEvent(event,
                   EVENT_TURN_LEFT_SINGLE |
                   EVENT_TURN_RIGHT_SINGLE |
                   EVENT_BUTTON_SHORT
                  )) {
    return false;
  }
  // button: next selection or save and exit
  if (SM_hasEvent(event, EVENT_BUTTON_SHORT)) {
    SM_selection = (SM_selection + 1 ) % 3;
    // save and exit
    if (SM_selection == 0) {
      uint8_t bpm[3];
      bpm[0] = round(SM_BPM[0]);
      bpm[1] = round(SM_BPM[1]);
      bpm[2] = round(SM_BPM[2]);
      EE_writeBPM(bpm);
      return true;
    }
  }
  SC_set(SM_selection * 7);
  //
  // single: set BPM
  if (SM_hasEvent(event, EVENT_TURN_LEFT_SINGLE)) {
    if (SM_BPM[SM_selection] > 1.0f) {
      SM_BPM[SM_selection] -= 1.0f;
      SM_printBPM(1);
    }
  }
  else if (SM_hasEvent(event, EVENT_TURN_RIGHT_SINGLE)) {
    if (SM_BPM[SM_selection] < 254.0f) {
      SM_BPM[SM_selection] += 1.0f;
      SM_printBPM(1);
    }
  }
  return false;
}

// ----------------------------------------------------------
void SM_processMetroScreen(uint8_t event) {
  if (!SM_hasEvent(event, EVENT_TURN_LEFT_MULTI | EVENT_TURN_RIGHT_MULTI)) {
    return;
  }
  if (SM_hasEvent(event, EVENT_TURN_LEFT_MULTI)) {
    SM_selection = SM_selection == 0 ? 2 : SM_selection - 1;
  }
  else if (SM_hasEvent(event, EVENT_TURN_RIGHT_MULTI)) {
    SM_selection = (SM_selection + 1 ) % 3;
  }
  SC_set(SM_selection * 7);
}

// ----------------------------------------------------------
void SM_initMetroSpeed() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("5':    3':    1':"));
  SM_printBPM(0);
  lcd.setCursor(0, 2);
  lcd.print(F("       START        "));
  SC_init(1, 3, 5);
  SC_set(7);
}

// ----------------------------------------------------------
void SM_processMetroSpeed(uint8_t event) {
  if (!SM_hasEvent(event,
                   EVENT_TURN_LEFT_SINGLE |
                   EVENT_TURN_RIGHT_SINGLE |
                   EVENT_BUTTON_SHORT
                  )) {
    return;
  }
  float bpmOffset = 5.0f;
  if (SM_hasEvent(event, EVENT_TURN_LEFT_SINGLE)) {
    if (SM_BPM[0] > bpmOffset) {
      bpmOffset = -5.0f;
    } else {
      bpmOffset = 0.0f;
    }
  }
  else if (SM_hasEvent(event, EVENT_TURN_RIGHT_SINGLE)) {
    if (SM_BPM[0] + bpmOffset > 255.0f) {
      bpmOffset = 0;
    }
  }
  else {
    bpmOffset = 0;
  }
  //
  float prevBpm = SM_BPM[0];
  SM_BPM[0] += bpmOffset;
  SM_BPM[1] *= SM_BPM[0] / prevBpm;
  SM_BPM[2] *= SM_BPM[0] / prevBpm;
  SM_printBPM(0);
  if (SM_hasEvent(event, EVENT_BUTTON_SHORT)) {
    uint8_t bpm[3];
    bpm[0] = round(SM_BPM[0]);
    bpm[1] = round(SM_BPM[1]);
    bpm[2] = round(SM_BPM[2]);
    EE_writeBPM(bpm);
  }
}

// ----------------------------------------------------------
void SM_initMetroGetReady() {
  lcd.clear();
  uint8_t mode = (5 - SM_time531) / 2;
  if (SM_531hasMetro) {
    sprintf(SM_lineBuffer, "GET READY FOR %d':%03d", SM_time531, (int)round(SM_BPM[mode]));
    SM_lineBuffer[20] = 0;
  } else {
    sprintf(SM_lineBuffer, "  GET READY FOR %d'", SM_time531);
    SM_lineBuffer[18] = 0;
  }
  lcd.setCursor(0, 1);
  lcd.print(SM_lineBuffer);
  TB_set(10, 3);
}

// ----------------------------------------------------------
void SM_initMetroRun() {
  uint8_t mode = (5 - SM_time531) / 2;
  uint8_t ms[3];
  EE_read531Data(ms, mode);
  SM_timerMinutes = ms[0];
  SM_timerSeconds = ms[1];
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("REMAINING:");
  SM_printTime(SM_timerMinutes, SM_timerSeconds, 13, 1);
  PB_init(3, (float)SM_timerMinutes * 60.0f + (float)SM_timerSeconds);
  PB_set((float)SM_timerMinutes * 60.0f + (float)SM_timerSeconds);
  SM_timer0.set(1000, true);
  //
  // set BPM timer
  if (SM_531hasMetro) {
    float ms = 60000.0 / SM_BPM[mode];
    SM_timerBPM.set(round(ms), true);
    lcd.setBacklight(LOW);
    SM_backlight = false;
  }
}

// ----------------------------------------------------------
bool SM_processMetroRun() {
  if (SM_timerMinutes == 0 && SM_timerSeconds == 0) {
    SM_timer0.stop();
    SM_timerBPM.stop();
    lcd.setBacklight(HIGH);
    return true;
  }
  bool timeIsUp = false;
  //
  // Timer 0: time countdown
  if (SM_timer0.process()) {
    if (SM_timerSeconds == 0) {
      SM_timerSeconds = 60;
      SM_timerMinutes--;
    }
    SM_timerSeconds--;
    timeIsUp = !(SM_timerMinutes || SM_timerSeconds);
    if (timeIsUp) {
      SM_timer0.stop();
      SM_timerBPM.stop();
      lcd.setBacklight(HIGH);
    }
    SM_printTime(SM_timerMinutes, SM_timerSeconds, 13, 1);
    PB_set((float)SM_timerMinutes * 60.0f + (float)SM_timerSeconds);
  }
  //
  // TimerBMP: backlight flashing
  if (SM_531hasMetro) {
    unsigned long now = millis();
    if (SM_timerBPM.process()) {
      SM_backlight = true;
      lcd.setBacklight(HIGH);
      SM_backlightOffTime = now + SM_BACKLIGHT_FLASHTIME;
    }
    if (SM_backlight && now >= SM_backlightOffTime) {
      SM_backlight = false;
      lcd.setBacklight(LOW);
    }
  }
  return timeIsUp;
}

// ----------------------------------------------------------
void SM_initMetroFinished() {
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print(F("SESSION FINISHED"));
  lcd.setCursor(2, 2);
  lcd.print(F("REPEAT"));
  lcd.setCursor(13, 2);
  lcd.print(F("EXIT"));
  SC_init(1, 3, 6);
  SC_set(12);
  SM_selection = 1;
}

// ----------------------------------------------------------
void SM_processMetroFinished(uint8_t event) {
  if (!SM_hasEvent(event, EVENT_TURN_LEFT_MULTI | EVENT_TURN_RIGHT_MULTI)) {
    return;
  }
  SM_selection = (SM_selection + 1) % 2;
  SC_set(2 + SM_selection * 10);
}
