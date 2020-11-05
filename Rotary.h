#define RT_PIN_CLK 5
#define RT_PIN_DT 6
#define RT_PIN_BUTTON 7
#define RT_PIN_GND 9
#define RT_PIN_VCC 8
//
#define RT_STATE_1_1 0
#define RT_STATE_0_0 2
#define RT_STATE_DIFF_A 1
#define RT_STATE_DIFF_B 3
//
#define RT_EVENT_TIME_DIFF_MIN 100
#define RT_THRESHTIME_LONGPRESS 1000

uint8_t RT_clk;
uint8_t RT_dt;
uint8_t RT_button;
uint8_t RT_state;
int8_t RT_rotDir;
uint8_t RT_multiCnt;
unsigned long RT_timeLastEvent;
bool RT_buttonPressed;
unsigned long RT_timeButtonChanged;

// ----------------------------------------------------------
void RT_init() {
  // rotary gets its power from I/O pins
  pinMode(RT_PIN_GND, OUTPUT);
  pinMode(RT_PIN_VCC, OUTPUT);
  digitalWrite(RT_PIN_GND, LOW);
  digitalWrite(RT_PIN_VCC, HIGH);
  //
  pinMode(RT_PIN_CLK, INPUT);
  pinMode(RT_PIN_DT, INPUT);
  pinMode(RT_PIN_BUTTON, INPUT);
  RT_clk = HIGH;
  RT_dt = HIGH;
  RT_multiCnt = 0;
  RT_state = RT_STATE_1_1;
  RT_rotDir = 0;
  RT_timeLastEvent = millis();
  RT_buttonPressed = false;
}

// ----------------------------------------------------------
bool RT_isAllHigh() {
  return RT_clk && RT_dt;
}

// ----------------------------------------------------------
void RT_readAll() {
  RT_clk = digitalRead(RT_PIN_CLK);
  RT_dt = digitalRead(RT_PIN_DT);
  RT_button = digitalRead(RT_PIN_BUTTON);
}

// ----------------------------------------------------------
inline void RT_switchState(uint8_t state) {
  RT_state = state;
}

// ----------------------------------------------------------
uint8_t RT_processRotary() {
  uint8_t resultEvent = EVENT_NULL;
  RT_readAll();
  unsigned long now = millis();
  switch (RT_state) {
    //
    // 1_1, the IDLE state. - - - - - - - -
    case RT_STATE_1_1:
      if (RT_clk != RT_dt) {
        RT_rotDir = RT_dt ? 1 : -1;
        RT_switchState(RT_STATE_DIFF_A);
      }
      break;
    //
    // 0_1 or 1_0, rotation states - - - - -
    case RT_STATE_DIFF_A:
      // rotated to next state?
      // 0_0: we are good to go.
      // 1_1: it is a rebound. invalidate and change to IDLE.
      if (RT_clk == RT_dt) {
        if (RT_clk == 1) {
          RT_rotDir = 0;
          RT_switchState(RT_STATE_1_1);
        } else {
          RT_switchState(RT_STATE_0_0);
        }
      }
      break;
    //
    // 0_0, the transition state - - - - - -
    case RT_STATE_0_0:
      // rotated to next state?
      // A != B: must be opposit direction, then it's ok
      if (RT_clk != RT_dt) {
        if (RT_dt != RT_rotDir) {
          RT_switchState(RT_STATE_DIFF_B);
        } else {
          RT_switchState(RT_STATE_DIFF_A);
        }
      }
      else if (RT_dt == 1) {
        RT_rotDir = 0;
        RT_switchState(RT_STATE_1_1);
      }
      break;
    // inverted diff state - - - - - -
    case RT_STATE_DIFF_B:
      if (RT_dt == RT_clk) {
        if (RT_dt == 0) {
          RT_switchState(RT_STATE_0_0);
        }
        // => we are back to 1_1
        else {
          //
          // fire event(s)
          // single rotation event
          resultEvent = RT_rotDir == -1 ? EVENT_TURN_LEFT_SINGLE : EVENT_TURN_RIGHT_SINGLE;
          Serial.println("Single event !!!");
          // multi rotation event
          if (now <= RT_timeLastEvent + RT_EVENT_TIME_DIFF_MIN) {
            RT_multiCnt += 1;
            if (RT_multiCnt == 3){
              resultEvent |= RT_rotDir == -1 ? EVENT_TURN_LEFT_MULTI : EVENT_TURN_RIGHT_MULTI;
              Serial.println("Multi event !!!");
            }
          } else {
            RT_multiCnt = 0;
          }
          RT_timeLastEvent = now;
          // 1_1: change state
          RT_switchState(RT_STATE_1_1);
          RT_rotDir = 0;
        }
      }
      break;
  }
  return resultEvent;
}

// ----------------------------------------------------------
uint8_t RT_processButton() {
  uint8_t resultEvent = EVENT_NULL;
  unsigned long now = millis();
  if (RT_buttonPressed) {
    if (now > RT_timeButtonChanged + RT_THRESHTIME_LONGPRESS) {
      resultEvent = EVENT_BUTTON_LONG;
    }
    if ((now > RT_timeButtonChanged + RT_EVENT_TIME_DIFF_MIN) && digitalRead(RT_PIN_BUTTON)) {
      if (now < RT_timeButtonChanged + RT_THRESHTIME_LONGPRESS) {
        resultEvent = EVENT_BUTTON_SHORT;
      }
      RT_buttonPressed = false;
      RT_timeButtonChanged = millis();
    }
  } else {
    if ((now > RT_timeButtonChanged + RT_EVENT_TIME_DIFF_MIN) && !digitalRead(RT_PIN_BUTTON)) {
      RT_buttonPressed = true;
      RT_timeButtonChanged = millis();
    }
  }
  return resultEvent;
}

// ----------------------------------------------------------
uint8_t RT_process() {
  uint8_t resultEvent = RT_processRotary();
  resultEvent |= RT_processButton();
  return resultEvent;
}
