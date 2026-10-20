# CODE BLUE X — Medical AI Emergency Simulator

**Arduino UNO R4 WiFi + iPad + Render Cloud + optional OpenAI. No servo.**

This is a **fictional educational emergency-response game**, not a clinical tool. All vitals are simulated. Designed for medical and pharmacy student demonstrations. Three sample cases: anaphylaxis, hypoglycemia and suspected stroke. Answers and scores are game mechanics, not clinical credentialing.

## Parts
- Arduino UNO R4 **WiFi** (not Minima)
- 2 momentary buttons (optional; D2 and D3)
- 1 passive piezo buzzer (optional; D6)
- USB-C cable + power bank or laptop
- iPad with Safari + internet
- **No external sensor required** for the base version. If you already have HC-SR04, use it as the sole sensor.

## Detailed wiring

| Component | Component pin | Arduino UNO R4 WiFi |
|---|---|---|
| HC-SR04 (optional) | VCC | 5V |
| HC-SR04 | GND | GND |
| HC-SR04 | TRIG | D9 |
| HC-SR04 | ECHO | D10 |
| Button A (optional) | terminal 1 | D2 |
| Button A | terminal 2 | GND |
| Button B (optional) | terminal 1 | D3 |
| Button B | terminal 2 | GND |
| Passive piezo (optional) | + | D6 |
| Passive piezo | − | GND |

**UNO R4 WiFi digital pins are 5 V logic.** Use a standard 5 V HC-SR04; do not connect its 5 V echo to 3.3 V-only boards. Buttons use internal pullups, so **no resistors required**. If no HC-SR04, the program still runs, but only dashboard topic buttons change cloud state. Button A changes the local LED icon; the cloud dashboard is the authoritative game control.

## Deploy Cloud Backend (Render)
1. Upload the project contents (not the ZIP itself) to a GitHub repository.
2. On https://render.com create **New > Web Service**, connect GitHub repository.
3. Runtime Node, Build Command: `npm install`, Start Command: `npm start`.
4. Set environment variable `DEVICE_TOKEN` to a long, unique secret.
5. Optional: `OPENAI_API_KEY` for real AI, and `OPENAI_MODEL` for a model your account supports. **Do not expose API keys in browser or GitHub.**
6. Deploy. Open the resulting HTTPS address in iPad Safari.

The dashboard works in **Demo Mode** without an OpenAI key. Render free services can sleep; warm the service before the exhibition. Some networks block device Wi-Fi or outbound TLS; test with the actual network and power supply.

## Arduino IDE
1. Install Arduino IDE 2.x and the **Arduino UNO R4 Boards** package via Boards Manager.
2. Select **Arduino UNO R4 WiFi** and its USB port.
3. The included sketch uses `WiFiS3` and `Arduino_LED_Matrix` from the board package. It does **not** need Arduino Cloud or ArduinoJson.
4. Edit `WIFI_SSID`, `WIFI_PASSWORD`, `CLOUD_HOST` (hostname only, e.g. `example.onrender.com`) and `DEVICE_TOKEN`.
5. Upload and open Serial Monitor at 115200 baud.
6. Wave a hand 5–25 cm from HC-SR04 to cycle AIRWAY/CIRCULATION/MEDICATION on the dashboard. A brief entry into range counts as one wave, with a cooldown; pull your hand away before the next wave.

**Arduino Cloud setup is NOT required.** The sketch sends HTTPS POST requests directly to the Node backend. For reliable public exhibitions, use a stable always-on hosting tier and keep a local demo fallback.

## Local test
Install Node.js 18+ then run:

```bash
npm start
```

Open http://localhost:3000 . The app serves its own static dashboard. Use the iPad via the public Render URL for the event.

Test the API from a computer:

```bash
curl http://localhost:3000/api/state
curl -X POST http://localhost:3000/api/action -H 'Content-Type: application/json' -d '{"action":"next"}'
curl -X POST http://localhost:3000/api/device -H 'Content-Type: application/json' -H 'X-Device-Token: YOUR_TOKEN' -d '{"distance":15,"gesture":"wave"}'
```

## Event run-of-show (1–2 minutes per player)
1. Show the animated ECG and vital signs (simulated).
2. Student reads the emergency case and chooses an action.
3. Dashboard gives an explanation and updates score.
4. Ask AI: “Why was that action prioritized?”
5. Wave hand in front of HC-SR04 to switch focus category.
6. Tap NEXT PATIENT for a new case.

## Important limitations and security
- This demo uses **in-memory** state. Restarting Render resets scores. Everyone opening the same server shares one game state, so use one iPad per live session.
- The browser action endpoints are intentionally unauthenticated for a supervised booth. **Do not use as a public production medical application**; add session auth, rate limiting and persistent storage first.
- `DEVICE_TOKEN` protects only `/api/device`; set it on Render before connecting Arduino. Never commit real secrets.
- Cloud AI is optional; errors fall back to demo text. No patient data should be entered.
- The graph is a simulated ECG illustration, **not a heart measurement**.
- AI responses may be incorrect; a qualified educator should review teaching material before public use.


## Gemini AI version (updated)
This version uses Google Gemini instead of OpenAI. On Render > Environment set:
- `DEVICE_TOKEN`: long random string; must match the Arduino sketch.
- `GEMINI_API_KEY`: your key from https://aistudio.google.com/api-keys (keep secret; never put it in GitHub or Arduino).
- `GEMINI_MODEL`: `gemini-2.5-flash` or another model your Gemini API account supports.

Render build: `npm install`; start: `npm start`. No additional npm packages are required.
Without a Gemini key, or if the API fails, the educational demo response is used. Free-tier availability and limits vary.
The AI responses are for fictional educational simulations, not medical decisions.
