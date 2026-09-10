#include <Arduino.h>
#include <WiFiManager.h>

#include "AerialAlertsClient.h"
#include "Button.h"

#define LED_BUILTIN 8
#define BUTTON1_PIN 5
#define BUTTON2_PIN 6

const unsigned long BLINK_INTERVAL_MS = 1000;

WiFiManager wifiManager;
// AerialAlertsClient alertsClient("https://ubilling.net.ua/aerialalerts/", 15000);
Button button1(BUTTON1_PIN);
Button button2(BUTTON2_PIN);

unsigned long lastBlinkMs = 0;
bool ledState = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  button1.begin();
  button1.onClick([]() { Serial.println("Button 1: click"); });
  button1.onLongPress([]() { Serial.println("Button 1: long press"); });

  button2.begin();
  button2.onClick([]() { Serial.println("Button 2: click"); });
  button2.onLongPress([]() { Serial.println("Button 2: long press"); });

  // Tries stored SSID/password from NVS first; if none or connection
  // fails, starts a SoftAP + web portal to let the user configure WiFi.
  //wifiManager.autoConnect("u-lamp-setup");
}

void loop() {
  unsigned long now = millis();

  if (now - lastBlinkMs >= BLINK_INTERVAL_MS) {
    lastBlinkMs = now;
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
  }

  button1.loop();
  button2.loop();
  //alertsClient.loop();
}
