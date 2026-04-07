#pragma once

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "timer.h"
#include "stats.h"

class Renderer {
public:
  Renderer();
  bool begin();

  void drawIdle(int sessionNum, Stats& stats);
  void drawWork(Timer& t, int sessionNum, bool paused);
  void drawBreak(Timer& t, bool isLong, bool paused);
  void drawAlert(bool workDone, int sessionNum);
  void drawSessionDone(int sessionNum, Stats& stats);
  void drawFlash(bool white);
  void drawHoldBar(float pct);

private:
  Adafruit_SSD1306 _display;
  int              _tomatoFrame;

  void _drawTomato(int cx, int cy, int r, int frame);
  void _drawArcProgress(int cx, int cy, int r, float pct, bool paused);
  void _drawBigTime(int mm, int ss, int y);
  void _drawSessionDots(int current, int total, int x, int y);
  void _drawCentered(const String& text, int y, uint8_t size = 1);
  void _drawProgressBar(int x, int y, int w, int h, float pct);
};
