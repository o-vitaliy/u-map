#include "AerialAlertsClient.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

AerialAlertsClient::AerialAlertsClient(const char *url, unsigned long pollIntervalMs)
    : url_(url), pollIntervalMs_(pollIntervalMs), lastPollMs_(0) {}

void AerialAlertsClient::loop() {
  unsigned long now = millis();
  if (WiFi.status() == WL_CONNECTED && now - lastPollMs_ >= pollIntervalMs_) {
    lastPollMs_ = now;
    poll();
  }
}

void AerialAlertsClient::poll() {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  if (!http.begin(client, url_)) {
    Serial.println("Aerial alerts: failed to begin request");
    return;
  }

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    parseResponse(http.getString());
  } else {
    Serial.printf("Aerial alerts: request failed: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void AerialAlertsClient::parseResponse(const String &payload) {
  // 25 regions * ~65 bytes/entry (JsonObject overhead + key/value) fits well
  // under this with room to spare.
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.printf("Aerial alerts: JSON parse failed: %s\n", err.c_str());
    return;
  }

  JsonObject states = doc["states"].as<JsonObject>();
  if (states.isNull()) {
    Serial.println("Aerial alerts: response has no \"states\" object");
    return;
  }

  regions_.clear();
  regions_.reserve(states.size());
  for (JsonPair kv : states) {
    RegionAlert region;
    region.name = kv.key().c_str();
    region.alertNow = kv.value()["alertnow"] | false;
    regions_.push_back(region);
  }

  Serial.printf("Aerial alerts: parsed %u regions\n", regions_.size());
  for (const RegionAlert &region : regions_) {
    Serial.printf("  %s: %s\n", region.name.c_str(), region.alertNow ? "ALERT" : "clear");
  }
}

bool AerialAlertsClient::anyAlertActive() const {
  for (const RegionAlert &region : regions_) {
    if (region.alertNow) {
      return true;
    }
  }
  return false;
}
