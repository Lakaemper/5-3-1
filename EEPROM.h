#include <EEPROM.h>

#define EE_DATASIZE 13
const uint8_t defaults[] = {
  11,         // size = num of data bytes excl. checksum
  8, 30,      // timer: min , sec
  5, 0, 80,   // 5-3-1 slow: min, sec, bpm
  3, 0, 90,   // 5-3-1 medium: min, sec, bpm
  1, 0, 100,  // 5-3-1 fast: min, sec, bpm
  72          // checksum (all values (incl size) % 256)
};


bool EE_isValid();
uint8_t EE_generateChecksum();

// -------------------------------------------------------------------
// Check if EEPROM has valid entries. If not, initialize with default.
void EE_init() {
  EEPROM.begin();
  // init test, write defaults
  // uncomment next line to force defaults to be written to EEPROM
  //EEPROM.write(5,42);
  if (!EE_isValid()) {
    Serial.println(F("Initializing EEPROM with default values"));
    for (int i = 0; i < defaults[0] + 2; i++) {
      EEPROM.write(i, defaults[i]);
    }
  } else {
    Serial.println(F("Check: EEPROM valid."));
  }
}

// -------------------------------------------------------------------
// check if EEPROM is correctly initialized
bool EE_isValid() {
  if (EEPROM.read(0) != defaults[0]) return false;
  // checksum test
  uint8_t cs = EE_generateChecksum();
  Serial.print(F("Checksum is ")); Serial.println(cs);
  return cs == EEPROM.read(defaults[0] + 1);
}

// -------------------------------------------------------------------
uint8_t EE_generateChecksum() {
  uint8_t cs = 0;
  for (int i = 0; i < defaults[0] + 1; i++) {
    cs += EEPROM.read(i);
  }
  return cs;
}

// -------------------------------------------------------------------
// uint8_t[2]: min = 0, sec = 1
void EE_readTimerData(uint8_t *min_sec) {
  min_sec[0] = EEPROM.read(1);
  min_sec[1] = EEPROM.read(2);
  return;
}

// -------------------------------------------------------------------
// uint8_t[2]: min = 0, sec = 1
void EE_writeTimerData(uint8_t *min_sec) {
  EEPROM.write(1, min_sec[0]);
  EEPROM.write(2, min_sec[1]);
  uint8_t cs = EE_generateChecksum();
  int csPos = EEPROM.read(0) + 1;
  EEPROM.write(csPos, cs);
  return;
}

// -------------------------------------------------------------------
// idx 5,8,11 => BPM[0,1,2]
void EE_readBPM(uint8_t *bpm) {
  for (int i = 0; i < 3; i++) {
    bpm[i] = EEPROM.read(5 + 3 * i);
  }
  return;
}

// -------------------------------------------------------------------
// BPM[0,1,2] => idx 5,8,11
void EE_writeBPM(uint8_t *bpm) {
  EEPROM.write(5, bpm[0]);
  EEPROM.write(8, bpm[1]);
  EEPROM.write(11, bpm[2]);
  uint8_t cs = EE_generateChecksum();
  int csPos = EEPROM.read(0) + 1;
  EEPROM.write(csPos, cs);

  return;
}

// -------------------------------------------------------------------
// uint8_t[3]: min = 0, sec = 1, bpm = 2
// mode = 0: slow, 1 = medium, 2 = fast
void EE_read531Data(uint8_t *min_sec_bpm, uint8_t mode) {
  min_sec_bpm[0] = EEPROM.read(3 + 3 * mode);
  min_sec_bpm[1] = EEPROM.read(4 + 3 * mode);
  min_sec_bpm[2] = EEPROM.read(5 + 3 * mode);
  return;
}
