# Wi-Fi Pager

A simple, cheap Wi-Fi pager for an elderly relative — it lights up, vibrates, and
chimes within ~1 second when a family member calls a dedicated secret number.
Dad's real phone number stays completely untouched.

**Hardware cost:** ~$26 (V1) · **Running cost:** ~$1/month (just the phone number) ·
**Battery life:** ~21 days on a 2000 mAh LiPo · **Setup:** ~2.5 hours

---

## Project status

| Part | Status |
|------|--------|
| **System design** ([`wifi-pager-design.md`](wifi-pager-design.md)) | ✅ Complete — approaches compared, BOM, wiring, firmware, cloud, build steps |
| **V1 firmware** ([`firmware/`](firmware/)) | 🟡 Reference build — structured for PlatformIO; **not yet verified on hardware** |
| **V1 cloud bridge** ([`cloud/`](cloud/)) | 🟡 Reference — Telnyx → Worker → HiveMQ; deploy + test before relying on it |
| **V2 hardware** ([`hardware/`](hardware/)) | 🟡 Design complete through schematic + BOM + enclosure; **not yet routed / fabricated** |

> ⚠️ This is a personal/experimental build. Verify wiring, credentials, and a full
> end-to-end call test before depending on it for anything important.

---

## How it works

```
Family calls the secret Telnyx number
  → Telnyx fires a webhook to a Cloudflare Worker (free tier)
    → Worker publishes MQTT to HiveMQ Cloud (free tier)
      → ESP32-C3 wakes from modem sleep, lights LEDs + motor + buzzer
        → Dad sees it and calls back on his normal phone
```

No app. No smartphone for dad. No configuration after setup. The pager just sits on
his nightstand and waits.

---

## Stack

| Layer | Service | Cost |
|-------|---------|------|
| Phone number | Telnyx DID | $1.00/month |
| Webhook bridge | Cloudflare Worker | Free |
| MQTT broker | HiveMQ Cloud | Free |
| Firmware | ESP32-C3 (Arduino + PubSubClient) | — |

**End-to-end latency (dial → alert): < 1 second** on a typical home Wi-Fi network.

---

## Hardware (V1)

| Component | Part | Cost |
|-----------|------|------|
| MCU | Seeed Studio XIAO ESP32-C3 | $4.99 |
| Battery | Adafruit LiPo 3.7V 2000mAh #2011 | $12.50 |
| Indicator | 5× 5mm red LED + 68Ω resistors | ~$1 |
| Driver | 2× PN2222A NPN transistor | ~$0.50 |
| Alert | Coin vibration motor (3V ERM) | $1.50 |
| Alert | Passive piezo buzzer (3V) | $0.80 |
| Housing | ABS project box ~100×60×25mm | $4.50 |

Red LEDs are intentional — white LEDs (Vf 3.0–3.4V) are barely visible on a 3.3V
supply; red LEDs at 2.0V run at 19mA each with a 68Ω resistor.

---

## Repository structure

```
wifi-pager/
├── README.md                 # this file
├── wifi-pager-design.md      # full V1 system design (BOM, wiring, firmware, cloud, build)
├── firmware/                 # V1 firmware — PlatformIO project (XIAO ESP32-C3)
│   ├── platformio.ini
│   └── src/
│       ├── main.cpp
│       └── config.example.h  # copy to config.h and fill in secrets (gitignored)
├── cloud/                    # Telnyx → Cloudflare Worker → HiveMQ bridge
│   ├── worker.js
│   ├── wrangler.toml
│   └── README.md
└── hardware/                 # V2 custom-PCB + enclosure design package
    ├── DESIGN.md             # ESP32-C6 electrical design, power tree, schematic
    ├── BOM.csv               # JLCPCB-format bill of materials
    ├── ENCLOSURE.md          # 3D-printed shell spec
    ├── enclosure/pager.scad  # parametric OpenSCAD model
    └── kicad/pager.py        # schematic as code (SKiDL → KiCad netlist)
```

---

## Quick start

1. **Read** [`wifi-pager-design.md`](wifi-pager-design.md) — full wiring diagram, BOM,
   and click-by-click cloud setup.
2. **Order parts** (Amazon + Adafruit, ~$26, 2–5 day shipping).
3. **Cloud:** follow [`cloud/README.md`](cloud/README.md) — buy a Telnyx DID, create a
   HiveMQ cluster, deploy the Worker.
4. **Firmware:** follow [`firmware/README.md`](firmware/README.md) — copy
   `config.example.h` → `config.h`, then `pio run -t upload`.
5. **Test:** call the number, watch it light up.

---

## Version roadmap

**V2** (design in [`hardware/`](hardware/)): ESP32-C6 (Wi-Fi 6 TWT for ~2× battery),
custom JLCPCB PCB, 2.9" e-ink status display, SK6812 RGB notification ring, physical
audio mute switch, web config + OTA.

**V3:** LTE-M cellular fallback, solar trickle charge, companion app, two-way ack button.

---

## License

[MIT](LICENSE).
