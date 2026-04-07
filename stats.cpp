#include "stats.h"

Stats::Stats()
  : _totalSessions(0), _todaySessions(0), _bestDay(0) {}

void Stats::begin() {
  EEPROM.begin(EEPROM_SIZE);
  uint8_t magic = EEPROM.read(EEPROM_MAGIC_ADDR);
  if (magic != EEPROM_MAGIC) {
    Serial.println("[STATS] First boot — init EEPROM");
    _totalSessions = 0;
    _todaySessions = 0;
    _bestDay       = 0;
    save();
  } else {
    _totalSessions = (uint16_t)EEPROM.read(EEPROM_SESSIONS_ADDR)
                   | ((uint16_t)EEPROM.read(EEPROM_SESSIONS_ADDR + 1) << 8);
    _bestDay       = EEPROM.read(EEPROM_BEST_ADDR);
    _todaySessions = 0;   // Today always starts fresh
    Serial.print("[STATS] Total=");
    Serial.print(_totalSessions);
    Serial.print(" Best=");
    Serial.println(_bestDay);
  }
}

void Stats::save() {
  EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC);
  EEPROM.write(EEPROM_SESSIONS_ADDR,     (uint8_t)(_totalSessions & 0xFF));
  EEPROM.write(EEPROM_SESSIONS_ADDR + 1, (uint8_t)(_totalSessions >> 8));
  EEPROM.write(EEPROM_BEST_ADDR, _bestDay);
  EEPROM.commit();
  Serial.println("[STATS] Saved");
}

void Stats::addSession() {
  _totalSessions++;
  _todaySessions++;
  if (_todaySessions > _bestDay) _bestDay = _todaySessions;
  save();
  Serial.print("[STATS] Session #");
  Serial.print(_totalSessions);
  Serial.print(" Today:");
  Serial.println(_todaySessions);
}

uint16_t Stats::getTotalSessions()  { return _totalSessions; }
uint8_t  Stats::getTodaySessions()  { return _todaySessions; }
uint8_t  Stats::getBestDay()        { return _bestDay; }
void     Stats::resetToday()        { _todaySessions = 0; }
