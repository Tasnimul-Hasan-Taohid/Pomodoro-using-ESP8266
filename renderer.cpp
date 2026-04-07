#include "renderer.h"

Renderer::Renderer()
  : _display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET), _tomatoFrame(0) {}

bool Renderer::begin() {
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    Serial.println("[RENDERER] Failed");
    return false;
  }
  _display.clearDisplay();
  _display.setTextColor(SSD1306_WHITE);
  _display.display();
  Serial.println("[RENDERER] OK");
  return true;
}

// ─── Flash (alert) ─────────────────────────────────────────────
void Renderer::drawFlash(bool white) {
  if (white) _display.fillScreen(SSD1306_WHITE);
  else       _display.clearDisplay();
  _display.display();
}

// ─── Idle / Title Screen ───────────────────────────────────────
void Renderer::drawIdle(int sessionNum, Stats& stats) {
  _display.clearDisplay();

  // Title
  _drawCentered("POMODORO", 1, 1);
  _display.drawFastHLine(0, 10, OLED_WIDTH, SSD1306_WHITE);

  // Tomato drawing (left side)
  _drawTomato(24, 36, 18, _tomatoFrame);
  _tomatoFrame = (_tomatoFrame + 1) % TOMATO_ANIM_FRAMES;

  // Right side info
  _display.setTextSize(1);

  // Session counter
  _display.setCursor(52, 14);
  _display.print("Session");
  _display.setCursor(52, 24);
  _display.setTextSize(2);
  _display.print(sessionNum);
  _display.setTextSize(1);
  _display.print("/");
  _display.print(SESSIONS_BEFORE_LONG);

  // Stats
  _display.setCursor(52, 44);
  _display.print("Today:");
  _display.print(stats.getTodaySessions());

  _display.setCursor(52, 54);
  _display.print("Best: ");
  _display.print(stats.getBestDay());

  // Hint
  _display.drawFastHLine(0, 55, OLED_WIDTH, SSD1306_WHITE);
  _display.setCursor(0, 57);
  _display.print("Press=start  Hold=skip");

  _display.display();
}

// ─── Work Screen ───────────────────────────────────────────────
void Renderer::drawWork(Timer& t, int sessionNum, bool paused) {
  _display.clearDisplay();

  // Header
  _display.fillRect(0, 0, OLED_WIDTH, 11, SSD1306_WHITE);
  _display.setTextColor(SSD1306_BLACK);
  _display.setTextSize(1);
  _display.setCursor(2, 2);
  _display.print("FOCUS");

  // Session dots top right (in header)
  _drawSessionDots(sessionNum, SESSIONS_BEFORE_LONG, 70, 2);

  _display.setTextColor(SSD1306_WHITE);

  // Arc progress ring (left)
  _drawArcProgress(26, 38, 22, t.progress(), paused);

  // Big time (right of arc)
  _drawBigTime(t.minutesLeft(), t.secondsPartLeft(), 14);

  // Paused label
  if (paused) {
    _drawCentered("-- PAUSED --", 50);
  } else {
    _display.setCursor(0, 57);
    _display.print("Press=pause  Hold=reset");
  }

  _display.display();
}

// ─── Break Screen ──────────────────────────────────────────────
void Renderer::drawBreak(Timer& t, bool isLong, bool paused) {
  _display.clearDisplay();

  // Header
  _display.fillRect(0, 0, OLED_WIDTH, 11, SSD1306_WHITE);
  _display.setTextColor(SSD1306_BLACK);
  _display.setTextSize(1);
  _display.setCursor(2, 2);
  _display.print(isLong ? "LONG BREAK" : "SHORT BREAK");
  _display.setTextColor(SSD1306_WHITE);

  // Progress bar (horizontal, full width)
  _drawProgressBar(4, 14, 120, 6, t.progress());

  // Big time centre
  _drawBigTime(t.minutesLeft(), t.secondsPartLeft(), 26);

  // Motivational label
  _display.setTextSize(1);
  if (isLong) {
    _drawCentered("You earned it!", 46);
  } else {
    _drawCentered("Breathe. Stretch.", 46);
  }

  if (paused) {
    _drawCentered("-- PAUSED --", 56);
  } else {
    _display.setCursor(0, 57);
    _display.print("Press=pause  Hold=skip");
  }

  _display.display();
}

// ─── Alert Screen (timer expired) ─────────────────────────────
void Renderer::drawAlert(bool workDone, int sessionNum) {
  _display.clearDisplay();

  if (workDone) {
    _drawCentered("WORK DONE!", 6, 2);
    String s = "Session " + String(sessionNum) + " complete";
    _drawCentered(s, 30);
    _drawCentered("Press for break", 44);
  } else {
    _drawCentered("BREAK OVER!", 6, 2);
    _drawCentered("Time to focus.", 30);
    _drawCentered("Press to start", 44);
  }

  _display.display();
}

// ─── Session Done Summary ──────────────────────────────────────
void Renderer::drawSessionDone(int completedSessions, Stats& stats) {
  _display.clearDisplay();

  // Big tomato earned
  _drawTomato(20, 32, 16, 0);

  _display.setTextSize(1);

  _display.setCursor(46, 8);
  _display.print("Round done!");

  _display.setCursor(46, 22);
  _display.print("Today: ");
  _display.setTextSize(2);
  _display.print(stats.getTodaySessions());
  _display.setTextSize(1);
  _display.print(" pom");

  _display.setCursor(46, 44);
  _display.print("Total: ");
  _display.print(stats.getTotalSessions());

  _display.setCursor(46, 54);
  _display.print("Best:  ");
  _display.print(stats.getBestDay());
  _display.print("/day");

  _display.display();
}

// ─── Hold bar overlay ──────────────────────────────────────────
void Renderer::drawHoldBar(float pct) {
  _display.fillRect(0, 56, OLED_WIDTH, 8, SSD1306_BLACK);
  _display.drawRect(0, 57, OLED_WIDTH, 6, SSD1306_WHITE);
  int fill = (int)(pct * (OLED_WIDTH - 2));
  if (fill > 0) _display.fillRect(1, 58, fill, 4, SSD1306_WHITE);
  _display.display();
}

// ─── Private: Tomato drawing ───────────────────────────────────
void Renderer::_drawTomato(int cx, int cy, int r, int frame) {
  // Body (filled circle)
  _display.fillCircle(cx, cy, r, SSD1306_WHITE);

  // Highlight (top-left shine)
  _display.fillCircle(cx - r/3, cy - r/3, r/5, SSD1306_BLACK);

  // Stem (top)
  _display.drawFastVLine(cx, cy - r - 3, 4, SSD1306_WHITE);

  // Leaf (animated)
  switch (frame % 4) {
    case 0:
      _display.drawLine(cx, cy - r - 1, cx - 4, cy - r - 4, SSD1306_WHITE);
      _display.drawLine(cx, cy - r - 1, cx + 4, cy - r - 4, SSD1306_WHITE);
      break;
    case 1:
      _display.drawLine(cx, cy - r - 1, cx - 5, cy - r - 3, SSD1306_WHITE);
      _display.drawLine(cx, cy - r - 1, cx + 3, cy - r - 5, SSD1306_WHITE);
      break;
    case 2:
      _display.drawLine(cx, cy - r - 1, cx - 4, cy - r - 5, SSD1306_WHITE);
      _display.drawLine(cx, cy - r - 1, cx + 4, cy - r - 3, SSD1306_WHITE);
      break;
    case 3:
      _display.drawLine(cx, cy - r - 1, cx - 3, cy - r - 5, SSD1306_WHITE);
      _display.drawLine(cx, cy - r - 1, cx + 5, cy - r - 4, SSD1306_WHITE);
      break;
  }
}

// ─── Private: Arc progress ring ────────────────────────────────
// Draws a circle ring that fills clockwise showing timer progress
void Renderer::_drawArcProgress(int cx, int cy, int r, float pct, bool paused) {
  // Outer ring (background)
  _display.drawCircle(cx, cy, r, SSD1306_WHITE);
  _display.drawCircle(cx, cy, r - 1, SSD1306_WHITE);

  // Fill arc using pixel plotting (0=top, clockwise)
  // We blank out the "remaining" portion by drawing black pixels
  float done    = pct;        // fraction completed
  float remain  = 1.0f - done;
  float startAngle = done * 2.0f * 3.14159f - 3.14159f / 2.0f; // start from top

  // Draw filled dots along completed arc
  int steps = 64;
  for (int i = 0; i <= (int)(done * steps); i++) {
    float angle = -3.14159f / 2.0f + ((float)i / steps) * 2.0f * 3.14159f;
    int px = cx + (int)((r - 3) * cos(angle));
    int py = cy + (int)((r - 3) * sin(angle));
    _display.fillCircle(px, py, 1, SSD1306_WHITE);
  }

  // Centre: time digits drawn by caller, but draw pause icon if paused
  if (paused) {
    _display.fillRect(cx - 5, cy - 6, 3, 12, SSD1306_WHITE);
    _display.fillRect(cx + 2, cy - 6, 3, 12, SSD1306_WHITE);
  }
}

// ─── Private: Big MM:SS time ───────────────────────────────────
void Renderer::_drawBigTime(int mm, int ss, int y) {
  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", mm, ss);
  _display.setTextSize(2);
  _drawCentered(String(buf), y, 2);
}

// ─── Private: Session dots ─────────────────────────────────────
// Shows small filled/outline circles for sessions 1..total
void Renderer::_drawSessionDots(int current, int total, int x, int y) {
  for (int i = 0; i < total; i++) {
    int dotX = x + i * 10;
    if (i < current) {
      _display.fillCircle(dotX, y + 3, 3, SSD1306_BLACK);
      _display.drawCircle(dotX, y + 3, 3, SSD1306_WHITE);
      // Filled dot = completed session (white inner)
      _display.fillCircle(dotX, y + 3, 2, SSD1306_WHITE);
    } else if (i == current) {
      // Current session (outline only, black fill)
      _display.fillCircle(dotX, y + 3, 3, SSD1306_BLACK);
      _display.drawCircle(dotX, y + 3, 3, SSD1306_WHITE);
    } else {
      // Future (tiny dot)
      _display.drawCircle(dotX, y + 3, 2, SSD1306_BLACK);
      _display.drawPixel(dotX, y + 3, SSD1306_WHITE);
    }
  }
}

// ─── Private: Centered text ────────────────────────────────────
void Renderer::_drawCentered(const String& text, int y, uint8_t size) {
  _display.setTextSize(size);
  _display.setTextColor(SSD1306_WHITE);
  int16_t x1, y1; uint16_t w, h;
  _display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int x = max(0, (int)((OLED_WIDTH - (int)w) / 2));
  _display.setCursor(x, y);
  _display.print(text);
}

// ─── Private: Progress bar ─────────────────────────────────────
void Renderer::_drawProgressBar(int x, int y, int w, int h, float pct) {
  _display.drawRect(x, y, w, h, SSD1306_WHITE);
  int fill = (int)(pct * (w - 2));
  if (fill > 0) _display.fillRect(x + 1, y + 1, fill, h - 2, SSD1306_WHITE);
}
