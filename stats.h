#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"

class Stats {
public:
  Stats();
  void begin();
  void save();

  void     addSession();
  uint16_t getTotalSessions();
  uint8_t  getTodaySessions();
  uint8_t  getBestDay();
  void     resetToday();        // Call on new day

private:
  uint16_t _totalSessions;
  uint8_t  _todaySessions;
  uint8_t  _bestDay;
};
