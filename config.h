#pragma once

// ─── OLED ───────────────────────────────────────────────────────
#define OLED_SDA          4        // D2
#define OLED_SCL          5        // D1
#define OLED_WIDTH        128
#define OLED_HEIGHT       64
#define OLED_RESET        -1
#define OLED_I2C_ADDR     0x3C

// ─── Button ─────────────────────────────────────────────────────
#define BUTTON_PIN        0        // D3 / GPIO0 — active LOW
#define DEBOUNCE_MS       50
#define HOLD_MS           1500     // Hold 1.5s to reset / skip

// ─── Pomodoro Durations (in seconds) ────────────────────────────
#define WORK_DURATION_SEC      1500   // 25 minutes
#define SHORT_BREAK_SEC         300   //  5 minutes
#define LONG_BREAK_SEC          900   // 15 minutes
#define SESSIONS_BEFORE_LONG      4   // Long break after 4 work sessions

// ─── Alert Flash ────────────────────────────────────────────────
#define ALERT_FLASH_COUNT    6        // How many times to flash OLED on timer end
#define ALERT_FLASH_MS     150        // Flash interval ms

// ─── Animations ─────────────────────────────────────────────────
#define TOMATO_ANIM_FRAMES   4        // Frames in idle tomato animation
#define TICK_MS             1000      // 1 second timer tick

// ─── EEPROM ─────────────────────────────────────────────────────
#define EEPROM_SIZE          32
#define EEPROM_MAGIC_ADDR     0
#define EEPROM_MAGIC       0xCA
#define EEPROM_SESSIONS_ADDR  1       // Total sessions completed (uint16)
#define EEPROM_BEST_ADDR      3       // Best daily sessions (uint8)

// ─── App States ─────────────────────────────────────────────────
enum PomState {
  STATE_IDLE,          // Title / ready screen
  STATE_WORK,          // Work session running
  STATE_WORK_PAUSED,   // Work session paused
  STATE_ALERT,         // Timer done — flashing alert
  STATE_SHORT_BREAK,   // Short break running
  STATE_LONG_BREAK,    // Long break running
  STATE_BREAK_PAUSED,  // Break paused
  STATE_DONE,          // Session complete summary
};

// ─── Debug ──────────────────────────────────────────────────────
#define DEBUG_BAUD  115200
