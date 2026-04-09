# 🍅 Pomodoro Timer (ESP8266 + OLED)

> A full Pomodoro productivity timer on an ESP8266 with a 0.96" OLED. Work sessions, short breaks, long breaks, session tracking, animated arc progress ring, tomato animation, and EEPROM stats. All with one button.

No phone. No notifications. Just focus.

---

## What is Pomodoro?

The Pomodoro Technique is a time management method:

1. Work for **25 minutes** (one Pomodoro 🍅)
2. Take a **5 minute** short break
3. After **4 Pomodoros**, take a **15 minute** long break
4. Repeat

---

## Features

| Feature | Details |
|---|---|
| 🍅 Animated Tomato | Idle screen with wiggling tomato animation |
| ⏱️ Arc Progress Ring | Circular fill shows work session progress |
| 📊 Progress Bar | Horizontal fill for break countdown |
| ⏸️ Pause / Resume | Press to pause any session |
| ⏭️ Skip | Hold to skip a session or break |
| 🔔 Flash Alert | OLED flashes when timer ends |
| 🔢 Session Dots | Shows which session you're on (1–4) |
| 📈 Today's Sessions | Tracks poms completed today |
| 🏆 Best Day | Best session count in a day, saved to EEPROM |
| 💾 EEPROM Stats | Total sessions and best day survive power-off |

---

## Hardware

### Components

| Part | Details |
|---|---|
| Board | ESP8266 — NodeMCU or Wemos D1 Mini |
| Display | 0.96" OLED, SSD1306, 128×64 px, I2C |
| Button | Momentary push button (normally open) |
| Power | Micro-USB 5V |

### Wiring

| ESP8266 Pin | Connected To | Notes |
|---|---|---|
| D1 (GPIO5) | OLED SCL | I2C Clock |
| D2 (GPIO4) | OLED SDA | I2C Data |
| 3V3 | OLED VCC | 3.3V only — not 5V |
| GND | OLED GND + Button | Ground |
| D3 (GPIO0) | Button → GND | Active LOW, internal pull-up |

> **No external button needed to test!** GPIO0 is the built-in FLASH button on all NodeMCU and D1 Mini boards.

---

## Project Structure

```
ESP8266-Pomodoro-OLED/
├── main.ino          ← Full state machine + app logic
├── config.h          ← All durations, pin defs, EEPROM layout
├── button.h / .cpp   ← Press, hold, hold progress
├── timer.h / .cpp    ← Countdown timer with pause/resume
├── stats.h / .cpp    ← Session counting + EEPROM persistence
├── renderer.h / .cpp ← All OLED screens and drawing
└── README.md
```

---

## Library Installation

**Sketch → Include Library → Manage Libraries**, install:

| Library | Author |
|---|---|
| Adafruit GFX | Adafruit |
| Adafruit SSD1306 | Adafruit |

No other external libraries needed.

---

## Flash & Run

1. Open `main.ino` in Arduino IDE
2. **Tools → Board** → `NodeMCU 1.0` or `LOLIN(Wemos) D1 Mini`
3. **Tools → Port** → select your COM port
4. Click **Upload**
5. Device boots to idle screen immediately

---

## Controls

### All Screens
| Action | Result |
|---|---|
| Short press | Context action (start / pause / resume / confirm) |
| Hold 1.5s | Context action (skip / reset) |

### By Screen

| Screen | Press | Hold |
|---|---|---|
| Idle | Start work session | Cycle session number |
| Work | Pause | Reset to idle |
| Work (paused) | Resume | Reset to idle |
| Alert | Confirm → go to break/work | — |
| Break | Pause | Skip break → start work |
| Break (paused) | Resume | Skip break → start work |

---

## Screens

### Idle Screen
```
┌──────────────────────────┐
│         POMODORO         │
│  ────────────────────    │
│   🍅    Session          │
│         1/4              │
│         Today: 3         │
│         Best:  7         │
│  Press=start  Hold=skip  │
└──────────────────────────┘
```

### Work Screen
```
┌──────────────────────────┐
│ FOCUS        ● ○ ○ ○     │  ← session dots
│  ╭─────╮                 │
│  │ ╱── │   24:33         │  ← arc progress + big time
│  │ arc │                 │
│  ╰─────╯                 │
│  Press=pause  Hold=reset │
└──────────────────────────┘
```

### Break Screen
```
┌──────────────────────────┐
│       SHORT BREAK        │
│  [████████░░░░░░░░░░]    │  ← progress bar
│                          │
│          04:45           │  ← big time
│                          │
│     Breathe. Stretch.    │
│  Press=pause  Hold=skip  │
└──────────────────────────┘
```

### Alert Screen (work done)
```
┌──────────────────────────┐
│                          │
│       WORK DONE!         │
│   Session 2 complete     │
│    Press for break       │
│                          │
└──────────────────────────┘
```

### Session Done Summary
```
┌──────────────────────────┐
│   🍅    Round done!      │
│         Today: 4 pom     │
│         Total: 47        │
│         Best:  7/day     │
└──────────────────────────┘
```

---

## Pomodoro Flow

```
        ┌──────────────────────────────────────────┐
        │                  IDLE                    │
        └──────────────────┬───────────────────────┘
                           │ Press
                           ▼
                    ┌──────────────┐
             ┌────▶│     WORK     │◀──────────────┐
             │      └──────┬───────┘               │
             │      expire │                       │
             │             ▼                       │
             │      ┌──────────────┐               │
             │      │    ALERT     │  Press        │
             │      │  (work done) │───────────────┤
             │      └──────────────┘               │
             │                                     │
             │      ┌──────────────────┐           │
             │      │  SHORT / LONG    │           │
             └──────│     BREAK        │           │
     Hold=skip      └────────┬─────────┘           │
                       expire│                     │
                             ▼                     │
                      ┌──────────────┐             │
                      │    ALERT     │ Press       │
                      │ (break done) │─────────────┘
                      └──────────────┘
```

After every 4 work sessions → long break instead of short break.

---

## Configuration

Edit `config.h` to change durations:

```cpp
// Work session length (default 25 min)
#define WORK_DURATION_SEC      1500

// Short break (default 5 min)
#define SHORT_BREAK_SEC         300

// Long break (default 15 min)
#define LONG_BREAK_SEC          900

// Sessions before long break (default 4)
#define SESSIONS_BEFORE_LONG      4

// Hold time to skip/reset (default 1.5s)
#define HOLD_MS                1500

// Alert flash count
#define ALERT_FLASH_COUNT         6
```

### Quick Test Mode

To test the full flow quickly, temporarily reduce durations:

```cpp
#define WORK_DURATION_SEC   10   // 10 seconds
#define SHORT_BREAK_SEC      5   //  5 seconds
#define LONG_BREAK_SEC      10   // 10 seconds
```

---

## Troubleshooting

**Display blank**
- Check SDA → D2 (GPIO4), SCL → D1 (GPIO5)
- Use 3.3V for OLED — not 5V
- Confirm I2C address `0x3C` in `config.h`

**Button not responding**
- GPIO0 is active LOW, internal pull-up enabled
- Button connects GPIO0 to GND
- Open Serial Monitor at 115200 baud — look for `[BTN]` and `[STATE]` logs

**Stats reset after upload**
- Stats survive soft resets and power cycles
- Reflashing the firmware writes new code but doesn't clear EEPROM
- If stats are wrong, do a manual EEPROM clear (add to setup, upload, remove, upload again):
  ```cpp
  for (int i = 0; i < 32; i++) EEPROM.write(i, 0); EEPROM.commit();
  ```

**Timer drifts over time**
- The timer uses `millis()` which is accurate to ~0.01%
- Over 25 minutes, drift is under 2 seconds — acceptable for Pomodoro use

---

## License

MIT — free to use, modify, and build on.

---

## Credits

Pomodoro Technique® by Francesco Cirillo.  
Built with ESP8266 Arduino core + Adafruit SSD1306.
