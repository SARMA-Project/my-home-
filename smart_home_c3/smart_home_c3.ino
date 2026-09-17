// =============================================================================
//  ESP32-C3 SuperMini — 6-Channel Smart Home with ESP RainMaker + Google Home
//  Board       : ESP32C3 Dev Module (or AirM2M / WeAct ESP32-C3)
//  Partition   : RainMaker 4MB No OTA (or Huge APP 3MB)
//  Integration : Works with Google Home (No Hub / No Nest speaker required!)
//
//  PIN MAP (ESP32-C3 SuperMini):
//    Relay 1 (Fan 1)          -> Relay: GPIO 0  | Wall Switch: GPIO 7
//    Relay 2 (Fan 2)          -> Relay: GPIO 1  | Wall Switch: GPIO 10
//    Relay 3 (LED Tube)       -> Relay: GPIO 3  | Wall Switch: GPIO 20
//    Relay 4 (LED Bulb)       -> Relay: GPIO 4  | Wall Switch: GPIO 21
//    Relay 5 (Night Lamp)     -> Relay: GPIO 5  | Wall Switch: GPIO 2
//    Relay 6 (Speaker Socket) -> Relay: GPIO 6  | Wall Switch: GPIO 8
//    Reset / Boot Button      -> GPIO 9 (Hold 3s = Wi-Fi reset, 10s = Factory reset)
// =============================================================================

#include <Arduino.h>
#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <Preferences.h>

// =============================================================================
// Hardware Configuration
// =============================================================================
#define RELAY_ON      LOW    // Active-LOW relay module
#define RELAY_OFF     HIGH
#define DEBOUNCE_MS   200

#define RESET_BUTTON_PIN  9  // BOOT button on ESP32-C3

// SoftAP Provisioning Credentials (NO Bluetooth used = ZERO crashes)
const char *service_name = "PROV_SmartHome";
const char *pop          = "12345678";

struct RelayChannel {
  const char   *name;
  uint8_t       relayPin;
  uint8_t       swPin;
  const char   *prefKey;
  bool          state;
  bool          swLastState;
  unsigned long swLastChange;
  Switch       *rmakerSwitch;
};

RelayChannel channels[] = {
  { "Fan 1",          0,  7,  "r1", false, false, 0, NULL },
  { "Fan 2",          1,  10, "r2", false, false, 0, NULL },
  { "LED Tube",       3,  20, "r3", false, false, 0, NULL },
  { "LED Bulb",       4,  21, "r4", false, false, 0, NULL },
  { "Night Lamp",     5,  2,  "r5", false, false, 0, NULL },
  { "Speaker Socket", 6,  8,  "r6", false, false, 0, NULL },
};
const int NUM_CHANNELS = sizeof(channels) / sizeof(channels[0]);

Preferences prefs;

// =============================================================================
// Relay Control Helper
// =============================================================================
void applyRelay(int idx, bool on, bool reportToCloud = true) {
  channels[idx].state = on;
  digitalWrite(channels[idx].relayPin, on ? RELAY_ON : RELAY_OFF);
  prefs.putBool(channels[idx].prefKey, on);
  Serial.printf("[RELAY] %-16s -> %s\n", channels[idx].name, on ? "ON" : "OFF");

  if (reportToCloud && channels[idx].rmakerSwitch != NULL) {
    channels[idx].rmakerSwitch->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, on);
  }
}

// =============================================================================
// RainMaker Cloud Callback (Triggered when toggled from App or Google Home)
// =============================================================================
void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx) {
  const char *device_name = device->getDeviceName();
  const char *param_name  = param->getParamName();

  if (strcmp(param_name, ESP_RMAKER_DEF_POWER_NAME) == 0) {
    bool newState = val.val.b;
    for (int i = 0; i < NUM_CHANNELS; i++) {
      if (strcmp(device_name, channels[i].name) == 0) {
        Serial.printf("[CLOUD] Command received for %s: %s\n", device_name, newState ? "ON" : "OFF");
        channels[i].state = newState;
        digitalWrite(channels[i].relayPin, newState ? RELAY_ON : RELAY_OFF);
        prefs.putBool(channels[i].prefKey, newState);
        param->updateAndReport(val);
        break;
      }
    }
  }
}

// =============================================================================
// Provisioning Event Handler
// =============================================================================
void sysProvEvent(arduino_event_t *sys_event) {
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
      Serial.println("\n=======================================================");
      Serial.println("  ESP RainMaker SoftAP Provisioning Ready!");
      Serial.printf ("  1. On Phone: Turn OFF Mobile Data\n");
      Serial.printf ("  2. Connect Phone Wi-Fi to: %s\n", service_name);
      Serial.printf ("  3. Open ESP RainMaker App -> Add Device -> Scan QR below\n");
      Serial.printf ("     (Or tap 'I don't have QR code', PoP: %s)\n", pop);
      Serial.println("=======================================================");
      WiFiProv.printQR(service_name, pop, "softap");
      break;

    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      Serial.println("\n[PROV] Wi-Fi Credentials received successfully!");
      WiFiProv.endProvision();
      break;

    case ARDUINO_EVENT_PROV_END:
      Serial.println("[PROV] Provisioning session ended.");
      break;

    default:
      break;
  }
}

// =============================================================================
// Physical Wall Switch Handler
// =============================================================================
void handleWallSwitches() {
  unsigned long now = millis();
  for (int i = 0; i < NUM_CHANNELS; i++) {
    bool currentReading = digitalRead(channels[i].swPin);
    if (currentReading != channels[i].swLastState) {
      if ((now - channels[i].swLastChange) > DEBOUNCE_MS) {
        channels[i].swLastState  = currentReading;
        channels[i].swLastChange = now;
        // Toggle the relay on any wall switch flip
        applyRelay(i, !channels[i].state, true);
      }
    }
  }
}

// =============================================================================
// Reset / Boot Button Handler (GPIO 9)
// =============================================================================
void handleResetButton() {
  if (digitalRead(RESET_BUTTON_PIN) == LOW) {
    delay(50);
    if (digitalRead(RESET_BUTTON_PIN) == LOW) {
      Serial.println("[BUTTON] Button pressed, timing hold duration...");
      unsigned long startTime = millis();
      while (digitalRead(RESET_BUTTON_PIN) == LOW) {
        delay(100);
      }
      unsigned long duration = millis() - startTime;

      if (duration > 10000) {
        Serial.println("[RESET] Holding > 10s: Full Factory Reset!");
        RMakerFactoryReset(2);
      } else if (duration > 3000) {
        Serial.println("[RESET] Holding > 3s: Wi-Fi Reset!");
        RMakerWiFiReset(2);
      }
    }
  }
}

// =============================================================================
// SETUP
// =============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=============================================");
  Serial.println("   ESP32-C3 6-Channel Smart Home (RainMaker) ");
  Serial.println("=============================================");

  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  prefs.begin("relays", false);

  // 1. Initialize GPIOs and restore previous states
  for (int i = 0; i < NUM_CHANNELS; i++) {
    pinMode(channels[i].relayPin, OUTPUT);
    pinMode(channels[i].swPin,    INPUT_PULLUP);

    channels[i].state       = prefs.getBool(channels[i].prefKey, false);
    channels[i].swLastState = digitalRead(channels[i].swPin);

    digitalWrite(channels[i].relayPin, channels[i].state ? RELAY_ON : RELAY_OFF);
    Serial.printf("[INIT] %-16s -> %s\n", channels[i].name, channels[i].state ? "ON" : "OFF");
  }

  // 2. Initialize ESP RainMaker Node
  Node my_node = RMaker.initNode("MyHome_C3");

  // 3. Create and attach all 6 Switch Devices
  for (int i = 0; i < NUM_CHANNELS; i++) {
    channels[i].rmakerSwitch = new Switch(channels[i].name, NULL, channels[i].state);
    if (channels[i].rmakerSwitch) {
      channels[i].rmakerSwitch->addCb(write_callback);
      my_node.addDevice(*(channels[i].rmakerSwitch));
      Serial.printf("[RMAKER] Registered Device: %s\n", channels[i].name);
    }
  }

  // 4. Enable RainMaker Timezone & Scheduling (Optional, powerful)
  RMaker.enableTZService();
  RMaker.enableSchedule();

  // 5. Initialize WiFi Provisioning with SoftAP ONLY (NO BLUETOOTH = ZERO CRASH)
  WiFiProv.initProvision(NETWORK_PROV_SCHEME_SOFTAP, NETWORK_PROV_SCHEME_HANDLER_NONE);

  // 6. Start RainMaker Agent
  RMaker.start();

  // 7. Register event listener and start SoftAP broadcast
  WiFi.onEvent(sysProvEvent);
  WiFiProv.beginProvision(
    NETWORK_PROV_SCHEME_SOFTAP,
    NETWORK_PROV_SCHEME_HANDLER_NONE,
    NETWORK_PROV_SECURITY_1,
    pop,
    service_name
  );

  Serial.println("[SETUP] Setup complete. Ready.");
}

// =============================================================================
// LOOP
// =============================================================================
void loop() {
  handleWallSwitches();
  handleResetButton();
  delay(10);
}
