#include <math.h>
#include "config.h"
#include "button.h"
#include "timer.h"
#include "stats.h"
#include "renderer.h"

// ─── Objects ───────────────────────────────────────────────────
Button   btn;
Timer    timer;
Stats    stats;
Renderer renderer;

// ─── App State ─────────────────────────────────────────────────
PomState  state          = STATE_IDLE;
int       sessionNum     = 0;    // 0-based, 0..SESSIONS_BEFORE_LONG-1
int       alertFlashes   = 0;
unsigned long alertStart = 0;
unsigned long lastRedraw = 0;
bool      alertIsWork    = false; // true=work ended, false=break ended

// ─── Helpers ───────────────────────────────────────────────────
void setState(PomState s) {
  state = s;
  const char* n[] = {
    "IDLE","WORK","WORK_PAUSED","ALERT",
    "SHORT_BREAK","LONG_BREAK","BREAK_PAUSED","DONE"
  };
  Serial.print("[STATE] -> ");
  Serial.println(n[(int)s]);
}

bool isBreakState() {
  return (state == STATE_SHORT_BREAK ||
          state == STATE_LONG_BREAK  ||
          state == STATE_BREAK_PAUSED);
}

bool isLongBreak() {
  return (state == STATE_LONG_BREAK || 
          (state == STATE_BREAK_PAUSED && sessionNum == 0));
}

void startWork() {
  timer.set(WORK_DURATION_SEC);
  timer.start();
  setState(STATE_WORK);
  Serial.print("[POMO] Work session ");
  Serial.println(sessionNum + 1);
}

void startBreak() {
  bool longBreak = (sessionNum == 0);
  if (longBreak) {
    timer.set(LONG_BREAK_SEC);
    setState(STATE_LONG_BREAK);
    Serial.println("[POMO] Long break");
  } else {
    timer.set(SHORT_BREAK_SEC);
    setState(STATE_SHORT_BREAK);
    Serial.println("[POMO] Short break");
  }
  timer.start();
}

void triggerAlert(bool workDone) {
  alertFlashes = 0;
  alertStart   = millis();
  alertIsWork  = workDone;
  setState(STATE_ALERT);
  Serial.println("[POMO] Alert!");
}

void doAlertFlash() {
  for (int i = 0; i < ALERT_FLASH_COUNT; i++) {
    renderer.drawFlash(true);
    delay(ALERT_FLASH_MS);
    renderer.drawFlash(false);
    delay(ALERT_FLASH_MS);
  }
  renderer.drawAlert(alertIsWork, sessionNum + 1);
}

// ──────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(DEBUG_BAUD);
  delay(100);
  Serial.println("\n\n== Pomodoro Timer v1.0 ==");

  btn.begin();

  if (!renderer.begin()) {
    Serial.println("[SETUP] Display FATAL");
    while (1) yield();
  }

  stats.begin();

  sessionNum = 0;
  setState(STATE_IDLE);
  renderer.drawIdle(sessionNum + 1, stats);

  Serial.println("[SETUP] Ready");
}

// ──────────────────────────────────────────────────────────────
void loop() {
  btn.update();
  unsigned long now = millis();

  // ── Timer tick (every ~1s, handled internally) ─────────────
  timer.tick();

  // ── Hold bar overlay while button held (all states) ────────
  if (btn.isCurrentlyHeld() && state != STATE_ALERT) {
    renderer.drawHoldBar(btn.holdProgress());
  }

  // ──────────────────────────────────────────────────────────
  // IDLE
  // ──────────────────────────────────────────────────────────
  if (state == STATE_IDLE) {
    // Animate tomato every 600ms
    if (now - lastRedraw >= 600) {
      lastRedraw = now;
      renderer.drawIdle(sessionNum + 1, stats);
    }

    if (btn.wasPressed()) {
      startWork();
    }

    if (btn.wasHeld()) {
      // Skip to next session (for testing)
      sessionNum = (sessionNum + 1) % SESSIONS_BEFORE_LONG;
      renderer.drawIdle(sessionNum + 1, stats);
    }

    yield(); return;
  }

  // ──────────────────────────────────────────────────────────
  // WORK
  // ──────────────────────────────────────────────────────────
  if (state == STATE_WORK) {
    // Redraw every second
    if (now - lastRedraw >= 1000) {
      lastRedraw = now;
      renderer.drawWork(timer, sessionNum + 1, false);
    }

    if (timer.isExpired()) {
      stats.addSession();
      triggerAlert(true);
      doAlertFlash();
      yield(); return;
    }

    if (btn.wasPressed()) {
      timer.pause();
      setState(STATE_WORK_PAUSED);
      renderer.drawWork(timer, sessionNum + 1, true);
    }

    if (btn.wasHeld()) {
      // Reset — back to idle, same session
      timer.reset();
      setState(STATE_IDLE);
      renderer.drawIdle(sessionNum + 1, stats);
    }

    yield(); return;
  }

  // ──────────────────────────────────────────────────────────
  // WORK PAUSED
  // ──────────────────────────────────────────────────────────
  if (state == STATE_WORK_PAUSED) {
    if (btn.wasPressed()) {
      timer.resume();
      setState(STATE_WORK);
      lastRedraw = now;
    }

    if (btn.wasHeld()) {
      // Reset to idle
      timer.reset();
      setState(STATE_IDLE);
      renderer.drawIdle(sessionNum + 1, stats);
    }

    yield(); return;
  }

  // ──────────────────────────────────────────────────────────
  // ALERT (flash done, waiting for button)
  // ──────────────────────────────────────────────────────────
  if (state == STATE_ALERT) {
    renderer.drawAlert(alertIsWork, sessionNum + 1);

    if (btn.wasPressed()) {
      if (alertIsWork) {
        // Work done → show session summary briefly, then break
        renderer.drawSessionDone(sessionNum + 1, stats);
        delay(2000);

        // Advance session counter
        sessionNum = (sessionNum + 1) % SESSIONS_BEFORE_LONG;
        startBreak();
        lastRedraw = now;
      } else {
        // Break done → back to work
        startWork();
        lastRedraw = now;
      }
    }

    yield(); return;
  }

  // ──────────────────────────────────────────────────────────
  // SHORT / LONG BREAK
  // ──────────────────────────────────────────────────────────
  if (state == STATE_SHORT_BREAK || state == STATE_LONG_BREAK) {
    bool longB = (state == STATE_LONG_BREAK);

    if (now - lastRedraw >= 1000) {
      lastRedraw = now;
      renderer.drawBreak(timer, longB, false);
    }

    if (timer.isExpired()) {
      triggerAlert(false);
      doAlertFlash();
      yield(); return;
    }

    if (btn.wasPressed()) {
      timer.pause();
      setState(STATE_BREAK_PAUSED);
      renderer.drawBreak(timer, longB, true);
    }

    if (btn.wasHeld()) {
      // Skip break — go straight to next work session
      timer.reset();
      startWork();
      lastRedraw = now;
    }

    yield(); return;
  }

  // ──────────────────────────────────────────────────────────
  // BREAK PAUSED
  // ──────────────────────────────────────────────────────────
  if (state == STATE_BREAK_PAUSED) {
    bool longB = isLongBreak();

    if (btn.wasPressed()) {
      timer.resume();
      setState(longB ? STATE_LONG_BREAK : STATE_SHORT_BREAK);
      lastRedraw = now;
    }

    if (btn.wasHeld()) {
      // Skip break
      timer.reset();
      startWork();
      lastRedraw = now;
    }

    yield(); return;
  }

  yield();
}
