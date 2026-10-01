#include "AerialAlertsClient.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

AerialAlertsClient::AerialAlertsClient(const char *url, unsigned long pollIntervalMs)
    : url_(url),
      pollIntervalMs_(pollIntervalMs),
      regionsMutex_(xSemaphoreCreateMutex()),
      pollingTask_(nullptr) {}

void AerialAlertsClient::loop() {
  if (pollingTask_ == nullptr) {
    startPollingTask();
  }
}

void AerialAlertsClient::startPollingTask() {
  BaseType_t created = xTaskCreate(pollingTaskEntry, "alerts-poll", 8192, this,
                                  1, &pollingTask_);
  if (created != pdPASS) {
    pollingTask_ = nullptr;
    Serial.println("Aerial alerts: failed to create polling task");
  }
}

void AerialAlertsClient::pollingTaskEntry(void *param) {
  AerialAlertsClient *self = static_cast<AerialAlertsClient *>(param);

  while (true) {
    if (WiFi.status() == WL_CONNECTED) {
      self->poll();
      vTaskDelay(pdMS_TO_TICKS(self->pollIntervalMs_));
    } else {
      vTaskDelay(pdMS_TO_TICKS(250));
    }
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
  // The configured regions * ~65 bytes/entry (JsonObject overhead + key/value)
  // fits well under this with room to spare.
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

  std::map<String, bool> updatedRegions;
  for (JsonPair kv : states) {
    updatedRegions[kv.key().c_str()] = kv.value()["alertnow"] | false;
  }

  const size_t regionCount = updatedRegions.size();
  Serial.printf("Aerial alerts: parsed %u regions\n", regionCount);
  for (const auto &region : updatedRegions) {
    Serial.printf("  %s: %s\n", region.first.c_str(), region.second ? "ALERT" : "clear");
  }

  if (xSemaphoreTake(regionsMutex_, portMAX_DELAY) == pdTRUE) {
    regions_.swap(updatedRegions);
    xSemaphoreGive(regionsMutex_);
  }
}

void AerialAlertsClient::copyRegionsTo(
  std::map<String, bool> &destination) const {
  if (xSemaphoreTake(regionsMutex_, portMAX_DELAY) == pdTRUE) {
    destination = regions_;
    xSemaphoreGive(regionsMutex_);
  }
}

bool AerialAlertsClient::anyAlertActive() const {
  std::map<String, bool> snapshot;
  copyRegionsTo(snapshot);
  for (const auto &region : snapshot) {
    if (region.second) {
      return true;
    }
  }
  return false;
}
