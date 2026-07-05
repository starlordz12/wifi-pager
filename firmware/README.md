# Firmware — Wi-Fi Pager V1

Reference firmware for the V1 pager (Seeed Studio **XIAO ESP32-C3**, Arduino
framework via PlatformIO). It connects to Wi-Fi, subscribes to an MQTT topic,
and on a "page" flashes the LEDs, buzzes the vibration motor, and chimes the
buzzer.

> **Status:** reference implementation extracted from [`../wifi-pager-design.md`](../wifi-pager-design.md).
> It is structured to build with PlatformIO, but has **not been verified on
> physical hardware here** — flash it, watch the serial log, and confirm the
> wiring self-test before relying on it.

## Layout

```
firmware/
├── platformio.ini        # board + library deps
└── src/
    ├── main.cpp          # pager logic
    └── config.example.h  # copy to config.h and fill in your secrets
```

## Build & flash

1. Install [PlatformIO](https://platformio.org/) (CLI or the VS Code extension).
2. Create your config:
   ```bash
   cp src/config.example.h src/config.h
   ```
   Edit `src/config.h` with your Wi-Fi and HiveMQ credentials. `config.h` is
   gitignored so secrets stay local.
3. Build and upload (XIAO connected over USB-C):
   ```bash
   pio run -t upload
   pio device monitor        # 115200 baud
   ```
4. On boot you should see the self-test fire (LEDs + motor + chime) and then
   `[READY] Waiting for pages...`. Publish to `pager/alert` (or place a test
   call once the cloud side is set up) to trigger an alert.

## Notes

- V1 uses `wifiClient.setInsecure()` — it connects to HiveMQ over TLS but does
  **not** validate the broker's certificate. Fine for a hobby build; pin the CA
  cert if you want full verification.
- Power saving uses `WIFI_PS_MIN_MODEM` (modem sleep, ~3–5 mA average) — see the
  battery-life math in the design doc.
- Pin map is in `config.h` (`PIN_LEDS`/`PIN_MOTOR`/`PIN_BUZZER`); wiring is in
  the design doc's wiring diagram.
