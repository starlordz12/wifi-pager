# Cloud — Telnyx → Cloudflare Worker → HiveMQ

The cloud side turns an inbound phone call into an MQTT "page." Nothing here
touches anyone's real phone number — it's a dedicated secret Telnyx number.

```
Family calls secret Telnyx number
  → Telnyx webhook  → Cloudflare Worker (worker.js)
    → publishes MQTT to HiveMQ  → pager/alert
      → ESP32 wakes and alerts
```

## Files

| File | What it is |
|------|------------|
| `worker.js` | The webhook bridge (Telnyx event → MQTT publish). |
| `wrangler.toml` | Cloudflare Worker config. Holds the non-secret HiveMQ hostname; secrets are set separately. |

## Deploy (~30 min)

1. **Telnyx** — create an account, add credit, buy one US local DID. Share it
   with family only (access control is "nobody else knows it exists").
2. **HiveMQ Cloud** — create a free Serverless cluster, add credentials, note the
   cluster hostname.
3. **Worker** — install [Wrangler](https://developers.cloudflare.com/workers/wrangler/),
   then from this folder:
   ```bash
   # set your cluster hostname in wrangler.toml (HIVEMQ_HOST), then:
   wrangler secret put HIVEMQ_USER      # paste your HiveMQ username
   wrangler secret put HIVEMQ_PASS      # paste your HiveMQ password
   wrangler deploy
   ```
4. **Wire Telnyx → Worker** — Telnyx console → Voice → Call Control Applications
   → create one → set the webhook URL to your deployed Worker URL → assign the DID.
5. **Test** — open the HiveMQ web client, subscribe to `pager/alert`, then call the
   Telnyx number. You should see a message land within ~1 second.

## Security note

- Secrets (`HIVEMQ_USER`, `HIVEMQ_PASS`) live in Cloudflare's secret store via
  `wrangler secret put`, **not** in this repo. Don't paste them into `wrangler.toml`.
- Consider verifying Telnyx's webhook signature in `worker.js` before trusting the
  event if you ever make the number less secret.
