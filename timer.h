#pragma once
#include <Arduino.h>

class Timer {
public:
  Timer();

  void  set(int totalSeconds);
  void  start();
  void  pause();
  void  resume();
  void  reset();

  void  tick();             // Call every ~1000ms

  bool  isRunning();
  bool  isPaused();
  bool  isExpired();

  int   secondsLeft();
  int   totalSeconds();
  float progress();         // 0.0 (start) → 1.0 (done)

  int   minutesLeft();
  int   secondsPartLeft();  // seconds component of remaining time

private:
  int           _total;
  int           _remaining;
  bool          _running;
  bool          _paused;
  bool          _expired;
  unsigned long _lastTick;
};
