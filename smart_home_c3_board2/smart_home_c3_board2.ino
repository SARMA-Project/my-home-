// =============================================================================
//  ESP32-C3 Smart Home — BOARD 2 (Channels 4 to 6) with Sinric Pro
//  Controls:
//    4. LED Bulb       -> Relay: GPIO 4  | Wall Switch: GPIO 21
//    5. Night light    -> Relay: GPIO 5  | Wall Switch: GPIO 2
//    6. Speaker        -> Relay: GPIO 6  | Wall Switch: GPIO 8
// =============================================================================

#include <Arduino.h>
#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <Preferences.h>

// =============================================================================
// 1. Wi-Fi Configuration
// =============================================================================
#define WIFI_SSID         "Airtel_AIR FIBER SARMA"
#define WIFI_PASS         "Sarma#123456789"

// =============================================================================
// 2. Sinric Pro ACCOUNT 2 Credentials
// =============================================================================
#define APP_KEY           "74981a6c-37c3-4f6f-896d-bd000dc6c25c"
#define APP_SECRET        "687bcced-2fe8-4c54-bc24-584f0104c011-64a2d370-3849-43da-a164-bdb664739c4b"

#define DEVICE_ID_LEDBULB "6aac135bb3889c3a11b70fc1"
#define DEVICE_ID_NIGHT   "6aac1337b597c4e1234a95c3"
#define DEVICE_ID_SPEAKER "6aac12fbb3889c3a11b70f05"

// =============================================================================
// 3. Hardware Pin Mapping & Logic
//    (Active-LOW Relay Module)
// =============================================================================
#define RELAY_ON          LOW
#define RELAY_OFF         HIGH
#define DEBOUNCE_MS       200

#define RELAY_PIN_LEDBULB 4
#define SW_PIN_LEDBULB    21

#define RELAY_PIN_NIGHT   5
#define SW_PIN_NIGHT      2

#define RELAY_PIN_SPEAKER 6
#define SW_PIN_SPEAKER    8

struct Channel {
  const char   *name;
  const char   *deviceId;
  uint8_t       relayPin;
  uint8_t       swPin;
  const char   *prefKey;
  bool          state;
  bool          swLastState;
  unsigned long swLastChange;
};

Channel channels[] = {
  { "LED Bulb",    DEVICE_ID_LEDBULB, RELAY_PIN_LEDBULB, SW_PIN_LEDBULB, "bulb",    false, false, 0 },
  { "Night light", DEVICE_ID_NIGHT,   RELAY_PIN_NIGHT,   SW_PIN_NIGHT,   "night",   false, false, 0 },
  { "Speaker",     DEVICE_ID_SPEAKER, RELAY_PIN_SPEAKER, SW_PIN_SPEAKER, "speaker", false, false, 0 }
};
const int NUM_CHANNELS = sizeof(channels) / sizeof(channels[0]);

Preferences prefs;

// =============================================================================
// Relay Control Function
// =============================================================================
void setRelay(int idx, bool on, bool reportToCloud = false) {
  channels[idx].state = on;
  digitalWrite(channels[idx].relayPin, on ? RELAY_ON : RELAY_OFF);
  prefs.putBool(channels[idx].prefKey, on);
  Serial.printf("[BOARD 2] %-14s -> %s\n", channels[idx].name, on ? "ON" : "OFF");

  if (reportToCloud) {
    SinricProSwitch &mySwitch = SinricPro[channels[idx].deviceId];
    mySwitch.sendPowerStateEvent(on);
  }
}

// =============================================================================
// Sinric Pro Cloud Callback
// =============================================================================
bool onPowerState(const String &deviceId, bool &state) {
  for (int i = 0; i < NUM_CHANNELS; i++) {
    if (deviceId == channels[i].deviceId) {
      Serial.printf("[CLOUD 2] Command for %s: %s\n", channels[i].name, state ? "ON" : "OFF");
      setRelay(i, state, false);
      return true;
    }
  }
  return false;
}

// =============================================================================
// Wall Switch Polling
// =============================================================================
void handleWallSwitches() {
  unsigned long now = millis();
  for (int i = 0; i < NUM_CHANNELS; i++) {
    bool reading = digitalRead(channels[i].swPin);
    if (reading != channels[i].swLastState) {
      if ((now - channels[i].swLastChange) > DEBOUNCE_MS) {
        channels[i].swLastState  = reading;
        channels[i].swLastChange = now;
        setRelay(i, !channels[i].state, true);
      }
    }
  }
}

// =============================================================================
// Wi-Fi Connection
// =============================================================================
void setupWiFi() {
  Serial.printf("\n[WIFI] Connecting to %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\n[WIFI] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
}

// =============================================================================
// Setup
// =============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=============================================");
  Serial.println("  ESP32-C3 Smart Home — BOARD 2 (Channels 4-6)");
  Serial.println("=============================================");

  prefs.begin("b2_relays", false);

  for (int i = 0; i < NUM_CHANNELS; i++) {
    pinMode(channels[i].relayPin, OUTPUT);
    pinMode(channels[i].swPin,    INPUT_PULLUP);

    channels[i].state       = prefs.getBool(channels[i].prefKey, false);
    channels[i].swLastState = digitalRead(channels[i].swPin);
    digitalWrite(channels[i].relayPin, channels[i].state ? RELAY_ON : RELAY_OFF);
    Serial.printf("[INIT] %-14s -> %s\n", channels[i].name, channels[i].state ? "ON" : "OFF");
  }

  setupWiFi();

  for (int i = 0; i < NUM_CHANNELS; i++) {
    SinricProSwitch &mySwitch = SinricPro[channels[i].deviceId];
    mySwitch.onPowerState(onPowerState);
  }

  SinricPro.onConnected([](){
    Serial.println("[SinricPro B2] Connected to Cloud Server successfully!");
  });
  SinricPro.onDisconnected([](){
    Serial.println("[SinricPro B2] Disconnected from Cloud Server!");
  });

  SinricPro.restoreDeviceStates(true);
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// =============================================================================
// Loop
// =============================================================================
void loop() {
  SinricPro.handle();
  handleWallSwitches();
}
