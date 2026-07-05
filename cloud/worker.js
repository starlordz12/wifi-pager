// Cloudflare Worker — Telnyx webhook -> HiveMQ MQTT bridge for the Wi-Fi Pager.
//
// Telnyx fires this webhook on an inbound call to the pager's secret number.
// The Worker publishes an MQTT message to `pager/alert`, which the ESP32 is
// subscribed to. Deploy with Wrangler (see README.md); set HIVEMQ_USER and
// HIVEMQ_PASS as secrets, and set HIVEMQ_HOST as a var/secret for your cluster.

export default {
  async fetch(request, env) {
    if (request.method !== 'POST') return new Response('OK', { status: 200 });

    let body;
    try {
      body = await request.json();
    } catch {
      return new Response('bad request', { status: 400 });
    }

    const eventType = body?.data?.event_type;
    if (eventType !== 'call.initiated' && eventType !== 'call.answered') {
      return new Response('ignored', { status: 200 });
    }

    const mqttPayload = JSON.stringify({
      event: 'incoming_call',
      from: body?.data?.payload?.from,
      timestamp: new Date().toISOString(),
    });

    await fetch(`https://${env.HIVEMQ_HOST}/api/v1/mqtt/publish`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        Authorization: `Basic ${btoa(`${env.HIVEMQ_USER}:${env.HIVEMQ_PASS}`)}`,
      },
      body: JSON.stringify({
        topic: 'pager/alert',
        payload: mqttPayload,
        qos: 1,
        retain: false,
      }),
    });

    // Tell Telnyx to speak a short confirmation, then the caller can hang up.
    return new Response(
      JSON.stringify({
        commands: [
          { command: 'speak', payload: 'Your page has been sent.', language: 'en-US' },
        ],
      }),
      { headers: { 'Content-Type': 'application/json' } }
    );
  },
};
