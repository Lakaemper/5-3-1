#ifndef LEDControl_H
#define LEDControl_H

#define TMC_STATE_IDLE 0
#define TMC_STATE_RUN 1

class Timer {
  public:
    uint8_t state;
    bool hasFired;
    bool repeat;
    unsigned long nextFire;
    unsigned long interval;

    Timer();
    void switchState(uint8_t state);
    uint8_t getEvent();
    void set(unsigned long timeMS, bool repeat);
    void stop();
    uint8_t process();
};
#endif

// ----------------------------------------------------------
Timer::Timer() {
  switchState(TMC_STATE_IDLE);
  hasFired = false;
  repeat = false;
}

// ----------------------------------------------------------
void Timer::switchState(uint8_t inState) {
  state = inState;
}

// ----------------------------------------------------------
uint8_t Timer::getEvent() {
  if (hasFired) {
    hasFired = false;
    return 1;
  } else {
    return  0;
  }
}

// ----------------------------------------------------------
void Timer::set(unsigned long timeMS, bool inRepeat) {
  nextFire = millis() + timeMS;
  repeat = inRepeat;
  interval = timeMS;
  switchState(TMC_STATE_RUN);
}

// ----------------------------------------------------------
void Timer::stop() {
  switchState(TMC_STATE_IDLE);
}

// ----------------------------------------------------------
uint8_t Timer::process() {
  switch (state) {
    case TMC_STATE_IDLE:
      break;
    case TMC_STATE_RUN:
      // below thresh time?
      if (millis() < nextFire) {
        break;
      }
      // time is up: fire
      hasFired = true;
      nextFire += interval;
      if (!repeat) {
        switchState(TMC_STATE_IDLE);
      }
      break;
  }
  return getEvent();
}
