#include "timer.h"

Timer::Timer()
  : _total(0), _remaining(0), _running(false),
    _paused(false), _expired(false), _lastTick(0) {}

void Timer::set(int totalSeconds) {
  _total     = totalSeconds;
  _remaining = totalSeconds;
  _running   = false;
  _paused    = false;
  _expired   = false;
  _lastTick  = 0;
}

void Timer::start() {
  _running  = true;
  _paused   = false;
  _expired  = false;
  _lastTick = millis();
}

void Timer::pause() {
  _running = false;
  _paused  = true;
}

void Timer::resume() {
  _running  = true;
  _paused   = false;
  _lastTick = millis();
}

void Timer::reset() {
  _remaining = _total;
  _running   = false;
  _paused    = false;
  _expired   = false;
}

void Timer::tick() {
  if (!_running || _expired) return;
  unsigned long now = millis();
  if (now - _lastTick >= 1000) {
    _lastTick = now;
    if (_remaining > 0) {
      _remaining--;
    }
    if (_remaining == 0) {
      _running = false;
      _expired = true;
    }
  }
}

bool  Timer::isRunning()  { return _running; }
bool  Timer::isPaused()   { return _paused; }
bool  Timer::isExpired()  { return _expired; }
int   Timer::secondsLeft(){ return _remaining; }
int   Timer::totalSeconds(){ return _total; }

float Timer::progress() {
  if (_total == 0) return 1.0f;
  return 1.0f - ((float)_remaining / (float)_total);
}

int Timer::minutesLeft() {
  return _remaining / 60;
}

int Timer::secondsPartLeft() {
  return _remaining % 60;
}
