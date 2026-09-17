// =============================================================================
//  ESP32-C3 Smart Home — BOARD 1 (Relays 1 to 3) with Sinric Pro
//  Controls:
//    1. Bedroom Fan 1   -> Relay: GPIO 0  | Wall Switch: GPIO 7
//    2. Bedroom Fan 2   -> Relay: GPIO 1  | Wall Switch: GPIO 10
//    3. LED Tube light  -> Relay: GPIO 3  | Wall Switch: GPIO 20
// =============================================================================

#include <Arduino.h>
#include <Preferences.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <WiFi.h>

// =============================================================================
// 1. Wi-Fi Configuration
// =============================================================================
#define WIFI_SSID "Airtel_AIR FIBER SARMA"
#define WIFI_PASS "Sarma#123456789"

// =============================================================================
// 2. Sinric Pro ACCOUNT 1 Credentials
// =============================================================================
#define APP_KEY "a79e7c74-4807-4b55-9150-92d948d403ee"
#define APP_SECRET                                                             \
  "e4571d07-0830-400d-b726-86bd89790846-fe237438-dc30-478c-9eb9-635b02a63524"

#define DEVICE_ID_FAN1 "6aac05e4b3889c3a11b6fe96"
#define DEVICE_ID_FAN2 "6aac061fb3889c3a11b6fee4"
#define DEVICE_ID_LEDTUBE "6aac067921568291be0c2470"

// =============================================================================
// 3. Hardware Pin Mapping & Logic
//    (Active-LOW Relay Module)
// =============================================================================
#define RELAY_ON LOW
#define RELAY_OFF HIGH
#define DEBOUNCE_MS 200

#define RELAY_PIN_FAN1 0
#define SW_PIN_FAN1 7

#define RELAY_PIN_FAN2 1
#define SW_PIN_FAN2 10

#define RELAY_PIN_LEDTUBE 3
#define SW_PIN_LEDTUBE 20

struct Channel {
  const char *name;
  const char *deviceId;
  uint8_t relayPin;
  uint8_t swPin;
  const char *prefKey;
  bool state;
  bool swLastState;
  unsigned long swLastChange;
};

Channel channels[] = {{"Bedroom Fan 1", DEVICE_ID_FAN1, RELAY_PIN_FAN1,
                       SW_PIN_FAN1, "fan1", false, false, 0},
                      {"Bedroom Fan 2", DEVICE_ID_FAN2, RELAY_PIN_FAN2,
                       SW_PIN_FAN2, "fan2", false, false, 0},
                      {"LED Tube light", DEVICE_ID_LEDTUBE, RELAY_PIN_LEDTUBE,
                       SW_PIN_LEDTUBE, "tube", false, false, 0}};
const int NUM_CHANNELS = sizeof(channels) / sizeof(channels[0]);

Preferences prefs;

// =============================================================================
// Relay Control Function
// =============================================================================
void setRelay(int idx, bool on, bool reportToCloud = false) {
  channels[idx].state = on;
  digitalWrite(channels[idx].relayPin, on ? RELAY_ON : RELAY_OFF);
  prefs.putBool(channels[idx].prefKey, on);
  Serial.printf("[BOARD 1] %-16s -> %s\n", channels[idx].name,
                on ? "ON" : "OFF");

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
      Serial.printf("[CLOUD 1] Command for %s: %s\n", channels[i].name,
                    state ? "ON" : "OFF");
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
        channels[i].swLastState = reading;
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
  Serial.printf("\n[WIFI] Connected! IP: %s\n",
                WiFi.localIP().toString().c_str());
}

// =============================================================================
// Setup
// =============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=============================================");
  Serial.println("  ESP32-C3 Smart Home — BOARD 1 (Channels 1-3)");
  Serial.println("=============================================");

  prefs.begin("b1_relays", false);

  for (int i = 0; i < NUM_CHANNELS; i++) {
    pinMode(channels[i].relayPin, OUTPUT);
    pinMode(channels[i].swPin, INPUT_PULLUP);

    channels[i].state = prefs.getBool(channels[i].prefKey, false);
    channels[i].swLastState = digitalRead(channels[i].swPin);
    digitalWrite(channels[i].relayPin,
                 channels[i].state ? RELAY_ON : RELAY_OFF);
    Serial.printf("[INIT] %-16s -> %s\n", channels[i].name,
                  channels[i].state ? "ON" : "OFF");
  }

  setupWiFi();

  for (int i = 0; i < NUM_CHANNELS; i++) {
    SinricProSwitch &mySwitch = SinricPro[channels[i].deviceId];
    mySwitch.onPowerState(onPowerState);
  }

  SinricPro.onConnected([]() {
    Serial.println("[SinricPro B1] Connected to Cloud Server successfully!");
  });
  SinricPro.onDisconnected([]() {
    Serial.println("[SinricPro B1] Disconnected from Cloud Server!");
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
