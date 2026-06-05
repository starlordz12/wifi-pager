# Wi-Fi Pager

A simple, cheap Wi-Fi pager for an elderly relative — lights up, vibrates, and chimes within ~1 second when a family member calls a dedicated secret number. Dad's real phone number is completely untouched.

**Hardware cost:** ~$24 one-time  
**Running cost:** ~$1/month (just the Telnyx phone number)  
**Battery life:** ~21 days on a 2000 mAh LiPo  
**Setup time:** ~2.5 hours

---

## How It Works

```
Family calls the secret Telnyx number
  → Telnyx fires a webhook to a Cloudflare Worker (free tier)
    → Worker publishes MQTT to HiveMQ Cloud (free tier)
      → ESP32-C3 wakes from modem sleep, activates LEDs + motor + buzzer
        → Dad sees it and calls back on his normal phone
```

No app. No smartphone for dad. No configuration after setup. The pager just sits on his nightstand and waits.

---

## Stack

| Layer | Service | Cost |
|-------|---------|------|
| Phone number | Telnyx DID | $1.00/month |
| Webhook bridge | Cloudflare Worker | Free |
| MQTT broker | HiveMQ Cloud | Free |
| Firmware | ESP32-C3 (Arduino + PubSubClient) | — |

---

## Hardware

| Component | Part | Cost |
|-----------|------|------|
| MCU | Seeed Studio XIAO ESP32-C3 | $4.99 |
| Battery | Adafruit LiPo 3.7V 2000mAh #2011 | $12.50 |
| Indicator | 5× 5mm red LED + 68Ω resistors | ~$1 |
| Driver | 2× PN2222A NPN transistor | ~$0.50 |
| Alert | Coin vibration motor (3V ERM) | $1.50 |
| Alert | Passive piezo buzzer (3V) | $0.80 |
| Housing | ABS project box ~100×60×25mm | $4.50 |
| **Total** | | **~$26** |

Red LEDs are intentional — white LEDs have a 3.0–3.4V forward voltage and are barely visible on a 3.3V supply. Red LEDs at 2.0V run at 19mA each with a 68Ω resistor.

---

## Repository Contents

| File | What it is |
|------|-----------|
| [`wifi-pager-design.md`](wifi-pager-design.md) | **Full design document** — complete BOM, wiring diagram, firmware, Cloudflare Worker code, step-by-step build instructions, V2/V3 roadmap |

The design doc is self-contained. Everything you need to build this is in there.

---

## Quick Start

1. Read [`wifi-pager-design.md`](wifi-pager-design.md) — it has the full wiring diagram, complete firmware, and exact click-by-click cloud setup.
2. Order the parts (Amazon + Adafruit, ~$24 total, 2–5 day shipping).
3. Create a free Telnyx account and buy one US local DID ($1/month). Share the number with family only.
4. Deploy the Cloudflare Worker (copy-paste, ~20 minutes).
5. Create a free HiveMQ Cloud cluster.
6. Upload the firmware to the XIAO ESP32-C3.
7. Test: call the number, watch it light up.

---

## Latency

End-to-end from dial to alert: **< 1 second** on a typical home Wi-Fi network.

| Step | Typical latency |
|------|----------------|
| Telnyx → Cloudflare Worker | < 500ms |
| Worker → HiveMQ MQTT publish | < 100ms |
| HiveMQ → ESP32 MQTT deliver | < 200ms |

---

## Version Roadmap

**V2:** ESP32-C6 (Wi-Fi 6 TWT for 2× battery life), custom PCB from JLCPCB, RGB NeoPixel ring, e-ink status display ("Last call: Mom, 2:34 PM"), Qi wireless charging dock.

**V3:** LTE-M cellular fallback, solar trickle charging, companion iOS/Android app, two-way ack button, commercial packaging.